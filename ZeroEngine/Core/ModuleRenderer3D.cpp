#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"

#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Core/glew/libx86/glew32.lib")

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	vsync_active = true;

	depth_test = true;
	cull_face = true;
	lighting = false;
	mat_color = true;
	texture = true;
	wireframe_mode = false;

	ray_cast = LineSegment();

}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
		
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum err = glewInit();

	LOG("Using Glew %s", glewGetString(GLEW_VERSION));
	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	

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

		//Initialize BlendFunc
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		//glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->editor_camera_info->frustum.pos.x, App->camera->editor_camera_info->frustum.pos.y, App->camera->editor_camera_info->frustum.pos.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{

	if (App->scene != nullptr)App->scene->draw = true;
	if (App->editor != nullptr)App->editor->draw = true;
	SDL_GL_SwapWindow(App->window->window);

	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::Update(float dt)
{
	
	DrawRayCast(ray_cast);

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
	glLoadMatrixf(App->camera->GetProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::VSYNC_() {

	ImGui::TextUnformatted("Render Options");
	if(ImGui::Checkbox("Depth Test", &depth_test))
		DrawingModes(depth_test, GL_DEPTH_TEST);
	
	if(ImGui::Checkbox("Cull Face", &cull_face))
		DrawingModes(cull_face, GL_CULL_FACE);

	if (ImGui::Checkbox("Lighting ON/OFF", &lighting))
		DrawingModes(lighting, GL_LIGHTING);

	if (ImGui::Checkbox("Color Material", &mat_color))
		DrawingModes(mat_color, GL_COLOR_MATERIAL);

	if (ImGui::Checkbox("Texture Draw", &texture))
		DrawingModes(texture, GL_TEXTURE_2D);

	if (ImGui::Checkbox("Wireframe Mode", &wireframe_mode))
		wireframe_mode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void ModuleRenderer3D::DrawingModes(bool currentState, int glMode) {
	currentState ? glEnable(glMode) : glDisable(glMode);
}

void ModuleRenderer3D::DrawRayCast(LineSegment ray_cast_) {

	glBegin(GL_LINES);

	glLineWidth(1.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	glVertex3f(ray_cast_.a.x, ray_cast_.a.y, ray_cast_.a.z);
	glVertex3f(ray_cast_.b.x, ray_cast_.b.y, ray_cast_.b.z);
	glEnd();

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
}

void ModuleRenderer3D::DrawBoxCollider(const float4x4& transform, const float3& half_size)
{
	glPushMatrix();
	glMultMatrixf(transform.Transposed().ptr());
	float4x4 pos = transform;
	float3 size, posi;
	Quat rot;
	pos.Decompose(posi, rot, size);
	//LOG("%f %f %f", posi.x, posi.y, posi.z);

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(-half_size.x, half_size.y, -half_size.z);
	glVertex3f(half_size.x, half_size.y, -half_size.z);
	glVertex3f(half_size.x, half_size.y, half_size.z);
	glVertex3f(-half_size.x, half_size.y, half_size.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(-half_size.x, -half_size.y, -half_size.z);
	glVertex3f(half_size.x, -half_size.y, -half_size.z);
	glVertex3f(half_size.x, -half_size.y, half_size.z);
	glVertex3f(-half_size.x, -half_size.y, half_size.z);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(-half_size.x, half_size.y, -half_size.z);
	glVertex3f(-half_size.x, -half_size.y, -half_size.z);
	glVertex3f(half_size.x, half_size.y, -half_size.z);
	glVertex3f(half_size.x, -half_size.y, -half_size.z);
	glVertex3f(half_size.x, half_size.y, half_size.z);
	glVertex3f(half_size.x, -half_size.y, half_size.z);
	glVertex3f(-half_size.x, half_size.y, half_size.z);
	glVertex3f(-half_size.x, -half_size.y, half_size.z);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void ModuleRenderer3D::DrawSphereCollider(const float4x4& transform, float radius) {
	glPushMatrix();
	glMultMatrixf(transform.Transposed().ptr());
	glColor3f(1.0f, 0.0f, 0.0f);

	float delta_amgle = 360.0f / 50.0f;  // Sphere sides
	float curr_angle = 0.f;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)50.0f; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), 0.0f, radius * sinf(DEGTORAD * curr_angle));
	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)50.0f; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), radius * sinf(DEGTORAD * curr_angle), 0.0f);
	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)50.0f; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(0.0f, radius * sinf(DEGTORAD * curr_angle), radius * cosf(DEGTORAD * curr_angle));
	}
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void ModuleRenderer3D::DrawCapsuleCollider(const float4x4& transform, const float half_height, const float radius) {
	glPushMatrix();

	float4x4 final_trams = transform * Quat::RotateZ(DEGTORAD * -90);
	glMultMatrixf(final_trams.Transposed().ptr());
	glColor3f(1.0f, 0.0f, 0.0f);

	float delta_amgle = 360.0f / 50.0f;
	float half_delta_angle = 180.f / 25.0f;
	float curr_angle = 0.f;


	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)50.0f; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), half_height, radius * sinf(DEGTORAD * curr_angle));
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= (int)25.0f; ++i) {
		curr_angle = half_delta_angle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), radius * sinf(DEGTORAD * curr_angle) + half_height, 0.0f);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= (int)25.0f; ++i) {
		curr_angle = half_delta_angle * i;
		glVertex3f(0.0f, radius * sinf(DEGTORAD * curr_angle) + half_height, radius * cosf(DEGTORAD * curr_angle));
	}
	glEnd();


	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)50.0f; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), -half_height, radius * sinf(DEGTORAD * curr_angle));
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= (int)25.0f; ++i) {
		curr_angle = 180.F + half_delta_angle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), radius * sinf(DEGTORAD * curr_angle) - half_height, 0.0f);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= (int)25.0f; ++i) {
		curr_angle = 180.F + half_delta_angle * i;
		glVertex3f(0.0f, radius * sinf(DEGTORAD * curr_angle) - half_height, radius * cosf(DEGTORAD * curr_angle));
	}
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(0.f, half_height, -radius);
	glVertex3f(0.f, -half_height, -radius);
	glVertex3f(0.f, half_height, radius);
	glVertex3f(0.f, -half_height, radius);
	glVertex3f(-radius, half_height, 0.f);
	glVertex3f(-radius, -half_height, 0.f);
	glVertex3f(radius, half_height, 0.f);
	glVertex3f(radius, -half_height, 0.f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void ModuleRenderer3D::RenderThrowSpheres(physx::PxRigidBody* sphere) {

	const PxVec3 color = PxVec3(0.0f, 0.75f, 0.0f);
	const PxMat44 shapePose(sphere->getGlobalPose());

	// render object
	glPushMatrix();
	glMultMatrixf(&shapePose.column0.x);
	float3 velocity = { sphere->getLinearVelocity().x, sphere->getLinearVelocity().y, sphere->getLinearVelocity().z };

	if (velocity.x == 0.0f && velocity.y == 0.0f && velocity.z == 0.0f)
	{
		const PxVec3 darkColor = color * 0.25f;
		glColor4f(darkColor.x, darkColor.y, darkColor.z, 1.0f);
	}
	else
		glColor4f(color.x, color.y, color.z, 1.0f);

	const PxF32 radius = 3.0f;

	App->physX->DrawGeometry(GeometryType::SPHERE, { NULL, NULL, NULL }, radius);

	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}