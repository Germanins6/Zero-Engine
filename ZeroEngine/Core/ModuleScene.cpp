#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "glew/include/glew.h"
#include "PrimitivesGL.h"
#include "ImGui/imgui.h"
#include "ModuleGeometry.h"
#include "Textures.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	draw = true;

}

ModuleScene::~ModuleScene()
{
	CleanUp();
}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	//Loading house and textures since beginning
	//App->resources->ImportFile("Assets/Models/BakerHouse.fbx");
	
	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{

	//Cleaning each gameObject parent , calling each gameObject destructor will clean also components.
	for (size_t i = 0; i < gameobjects.size(); i++)
	{
		delete gameobjects[i];
		gameobjects[i] = nullptr;
	}

	gameobjects.clear();

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

			//Just will check components and update if gameObject enabled
			if (gameobjects[i]->active) {
				for (size_t j = 0; j < gameobjects[i]->components.size(); j++) {
					if (gameobjects[i]->components[j] != nullptr)
						gameobjects[i]->components[j]->Update(dt);

				}

			}

			if(gameobjects[i]->GetMesh()!=nullptr)
				gameobjects[i]->Update(dt);
		}

	}
	return UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent) {

	GameObject* temp = new GameObject();
	gameobjects.push_back(temp);
	return temp;
}

GameObject* ModuleScene::CreateGameObject(PrimitiveTypesGL type, Mesh* data, GameObject* parent) {

	GameObject* temp = new GameObject(parent, data, type);
	gameobjects.push_back(temp);

	return temp;
}