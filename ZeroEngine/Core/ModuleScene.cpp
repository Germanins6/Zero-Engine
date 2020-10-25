#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "glew/include/glew.h"
#include "PrimitivesGL.h"
#include "ImGui/imgui.h"
#include "ModuleGeometry.h"
#include "ModuleTextures.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	draw = false;
	
	

	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleScene::Update(float dt)
{
	if (draw) {

		//Scene Grid
		App->primitivesGL->AxisGL();

		//Accesing all gameobjects in scene, then accesing each gameobject component and calling their updates
		for (size_t i = 0; i < gameobjects.size(); i++) {
			for (size_t i = 0; i < gameobjects[i]->children.size(); i++) {
				gameobjects[i]->children[i]->Update(dt);
				LOG("Detecting Component");
			}
			LOG("Detecting GO");

		}

	}

	return UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject() {

	GameObject* temp = new GameObject();
	gameobjects.push_back(temp);
	return temp;
}

GameObject* ModuleScene::CreateGameObject(const char* path) {

	GameObject* temp = new GameObject( nullptr , path);
	gameobjects.push_back(temp);
	return temp;
}