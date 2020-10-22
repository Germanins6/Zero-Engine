#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "glew/include/glew.h"
#include "PrimitivesGL.h"
#include "ImGui/imgui.h"
#include "ModuleGeometry.h"
#include "ModuleTextures.h"

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
	pyramid_pos = { 0.f,0.f,0.f };
	pyramid_size = { 1.f,1.f,1.f };
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	faces = 4;
	draw = false;
	warrior = new Mesh();
	//house = new Mesh();
	/*box = new CubeGL({ 1.f, 1.f, 1.f });
	sphere = new SphereGL(20, 20, 1.0f);
	pyramid = new PyramidGL(faces, { pyramid_size.x , pyramid_size.y, pyramid_size.z });
	cylinder = new CylinderGL(10, 1.f, 1.f);*/
	//App->geometry->LoadGeometry(warrior, "Assets/Models/BakerHouse.FBX");
	//App->geometry->LoadGeometry(house, "Assets/cube.fbx");

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

		//Plane p(0, 1, 0, 0);
		//p.axis = true;
		//p.Render(App->renderer3D->wireframe_mode);
		/*box->InnerRender({ 2, 0, 0 }, { 0,0,0,0 });
		sphere->InnerRender({ 5, 0, 0 }, { 0,0,0,0 });
		pyramid->InnerRender({ 6,0,0 }, { 0,0,0,0 });
		cylinder->InnerRender({ 8,0,0 }, { 0,0,0,0 });*/
		App->primitivesGL->AxisGL();

		//house->RenderGeometry(house);
		//warrior->RenderGeometry(warrior);
		//house->RenderGeometry(house);
	}
	return UPDATE_CONTINUE;
}

