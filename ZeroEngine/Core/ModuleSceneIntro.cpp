#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "glew/include/glew.h"
#include "PrimitivesGL.h"
#include "Primitive.h"
#include "ImGui/imgui.h"


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
	cube_pos = { 5,0,0 };
	pyramid_pos = { 0,0,0 };
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	faces = 4;
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
	if (draw) {

		Plane p(0, 1, 0, 0);
		p.axis = true;
		p.Render(App->renderer3D->wireframe_mode);
		
		//App->primitivesGL->CubeGL({ 1.f, 1.f, 1.f }, { cube_pos.x, cube_pos.y, cube_pos.z });
		//App->primitivesGL->SphereGL(100, 100, 1.0f, { 0.f, 0.f, 0.f });
		App->primitivesGL->PyramidGL(faces, { 1.f, 1.f, 1.f }, { pyramid_pos.x , pyramid_pos.y, pyramid_pos.z });
		//App->primitivesGL->CylinderGL();

	}
	return UPDATE_CONTINUE;
}

