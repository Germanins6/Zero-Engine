#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"

#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
//#pragma comment (lib, "Core/glew/libx86/glew32.lib") 
#pragma comment (lib, "Core/glew/libx86/glew32.lib")

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	vsync_active = false;

	depth_test = true;
	cull_face = true;
	lighting = true;
	mat_color = true;
	texture = true;
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	glewInit();

	if (vsync_active == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");

	}
	
	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
		cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
		lights[0].Active(true);
		lighting ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
		mat_color ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::VSYNC_() {

	ImGui::TextUnformatted("Render Options");
	ImGui::Checkbox("Depth Test", &depth_test);
	ImGui::SameLine();
	ImGui::Checkbox("Cull Face", &cull_face);
	ImGui::SameLine(); 
	ImGui::Checkbox("Lighting", &lighting);
	ImGui::SameLine();
	ImGui::Checkbox("Color Material", &mat_color);
	ImGui::SameLine();
	ImGui::Checkbox("Texture" , &texture);

	//Dummy bool
	bool wireframe = false;
	ImGui::Checkbox("Wireframe Mode", &wireframe);



	if (ImGui::Checkbox("VSYNC:", &vsync_active)) {
		
		if (vsync_active) { SDL_RENDERER_PRESENTVSYNC; }
		else { SDL_RENDERER_ACCELERATED; }

	}

	ImGui::SameLine();
	if(vsync_active)ImGui::TextColored(ImVec4(1, 1, 0, 1), "On");
	else{ ImGui::TextColored(ImVec4(1, 1, 0, 1), "Off"); }
}
