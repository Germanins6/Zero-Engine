#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "glew/include/glew.h"
#include "PrimitivesGL.h"
#include "ImGui/imgui.h"
#include "ModuleGeometry.h"
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
	
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	//Loading house and textures since beginning
	//App->resources->ImportFile("Assets/Models/BakerHouse.fbx");
	ModelImporter::Load("Library/Models/1055645526.ZeroModel"); 
	
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

void ModuleScene::SaveScene() const {

	Serializer scene;

	for (size_t i = 0; i < gameobjects.size(); i++)
	{
		scene.AddString("-Scene Name", name);

		scene.Object[to_string(i)];
		scene.AddStringObj("Name", gameobjects[i]->name, to_string(i));
		scene.AddUnsignedIntObj("ID", gameobjects[i]->uuid, to_string(i));
		scene.AddUnsignedIntObj("IDParent", gameobjects[i]->parentId, to_string(i));
		
		ComponentTransform* transform = dynamic_cast<ComponentTransform*>(gameobjects[i]->GetTransform());
		scene.AddFloat3Obj("Translation", transform->position, to_string(i));
		scene.AddQuaternionObj("Rotation", transform->rotation, to_string(i));
		scene.AddFloat3Obj("Scale", transform->scale, to_string(i));

		if (gameobjects[i]->GetMesh() != nullptr) {
			//Get resourceMesh UID -> MeshNonEqual to ResourceMesh fix that.
		}

		if (gameobjects[i]->GetMaterial() != nullptr) {

		}

		/*if (gameobjects[i]->GetCamera() != nullptr) {

		}*/
	}

	string path = "Assets/Scenes/" + name + ".ZeroScene";
	scene.Save(path.c_str());

}