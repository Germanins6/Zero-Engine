#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "glew/include/glew.h"
#include "PrimitivesGL.h"
#include "ImGui/imgui.h"
#include "Textures.h"
#include "Serialization.h"


#include "Serialization.h"
#include "ZeroImporter.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Scene";
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

	App->physX->CreateSphereCollider();

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

void ModuleScene::SaveScene() const {

	Serializer scene;

	for (size_t i = 0; i < gameobjects.size(); i++)
	{
		scene.AddString("-Scene Name", name);
		scene.AddUnsignedInt("-Num_Children", gameobjects.size() - 1);

		scene.Object[to_string(i)];
		scene.AddStringObj("Name", gameobjects[i]->name, to_string(i));
		scene.AddUnsignedIntObj("ID", gameobjects[i]->uuid, to_string(i));
		scene.AddUnsignedIntObj("IDParent", gameobjects[i]->parentId, to_string(i));
		
		ComponentTransform* transform = dynamic_cast<ComponentTransform*>(gameobjects[i]->GetTransform());
		scene.AddFloat3Obj("Translation", transform->position, to_string(i));
		scene.AddQuaternionObj("Rotation", transform->rotation, to_string(i));
		scene.AddFloat3Obj("Scale", transform->scale, to_string(i));

		if (gameobjects[i]->GetMesh() != nullptr) {
			UID meshUID = dynamic_cast<ComponentMesh*>(gameobjects[i]->GetMesh())->ourMesh->GetUID();
			scene.AddUnsignedIntObj("MeshUID", meshUID, to_string(i));
		}
		else {
			scene.AddUnsignedIntObj("MeshUID", 0, to_string(i));
		}

		if (gameobjects[i]->GetMaterial() != nullptr) {
			UID materialUID = dynamic_cast<ComponentMaterial*>(gameobjects[i]->GetMaterial())->GetMaterial()->GetUID();
			scene.AddUnsignedIntObj("MaterialUID", materialUID, to_string(i));
		}
		else {
			scene.AddUnsignedIntObj("MaterialUID", 0, to_string(i));
		}

		if (gameobjects[i]->GetCamera() != nullptr) {

		}
	}


	string path = "Assets/Scenes/" + name + ".ZeroScene";
	scene.Save(path.c_str());
}