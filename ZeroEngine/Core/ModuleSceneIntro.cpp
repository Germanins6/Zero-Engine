#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "glew/include/glew.h"
#include "PrimitivesGL.h"
ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	draw = false;
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update(float dt)
{
	if(draw==true){
		
		Plane p(0, 1, 0, 0);
		p.axis = true;
		p.Render(App->renderer3D->wireframe_mode);


		//Cube box(1, 1, 1);
		//box.Render(App->renderer3D->wireframe_mode);
		
		/*float cube[108]{

			//FRONT
			0.f, 0.f, 0.f,
			1.f, 0.f, 0.f,
			1.f, 1.f, 0.f,

			0.f, 0.f, 0.f,
			1.f, 1.f, 0.f,
			0.f, 1.f, 0.f,

			//LEFT			 
			0.f, 0.f,-1.f,
			0.f, 0.f, 0.f,
			0.f, 1.f, 0.f,

			0.f, 0.f,-1.f,
			0.f, 1.f, 0.f,
			0.f, 1.f,-1.f,

			//RIGHT	 
			1.f, 0.f, 0.f,
			1.f, 0.f, -1.f,
			1.f, 1.f, -1.f,

			1.f, 0.f, 0.f,
			1.f, 1.f, -1.f,
			1.f, 1.f, 0.f,

			//BACK
			0.f, 0.f, -1.f,
			1.f, 1.f, -1.f,
			1.f, 0.f, -1.f,

			0.f, 0.f, -1.f,
			0.f, 1.f, -1.f,
			1.f, 1.f, -1.f,

			//DOWN
			0.f, 0.f, 0.f,
			0.f, 0.f, -1.f,
			1.f, 0.f, -1.f,

			0.f, 0.f, 0.f,
			1.f, 0.f, -1.f,
			1.f, 0.f, 0.f,

			//UP
			1.f, 1.f, -1.f,
			0.f, 1.f, -1.f,
			0.f, 1.f, 0.f,

			1.f, 1.f, 0.f,
			1.f, 1.f, -1.f,
			0.f, 1.f, 0.f,

		};

		uint my_id = 0;
		glGenBuffers(1, (GLuint*)&(my_id));
		glBindBuffer(GL_ARRAY_BUFFER, my_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, cube, GL_STATIC_DRAW);

		glLineWidth(2.0f);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, my_id);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		//...bind and use other buffers
		glDrawArrays(GL_TRIANGLES, 0, 108);
		glDisableClientState(GL_VERTEX_ARRAY);
		*/
		
		App->primitivesGL->CubeGL({1.f, 2.f, 3.f});
	}
	return UPDATE_CONTINUE;
}

