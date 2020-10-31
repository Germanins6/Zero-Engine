#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"

//Empty GameObject constructor
GameObject::GameObject() {

	name = name + ("EmptyObject_");
	parent = nullptr;

	name += std::to_string(App->scene->gameobjects.size());

	CreateComponent(ComponentType::TRANSFORM);

	active = true;
}

//GameObject creator when mesh loaded
GameObject::GameObject(GameObject* owner, Mesh* data, const char* path) {

	//Set GameObject name depending path file info

	if (strlen(path) > 0) {
		//Set Name of GameObject
		name = App->file_system->SetNormalName(path);
		name = name.erase(name.size() - 4) + ("_");
		name += std::to_string(App->scene->gameobjects.size());
	}
	else name = "Empty GameObject";

	parent = owner;

	//Creating always once transform component
	CreateComponent(ComponentType::TRANSFORM);

	//Create Directly Mesh Component
	CreateComponent(ComponentType::MESH, path, data);

	//Create Material and assign texture from our file Textures if the current mesh data create as gO does have MatInfo
	if (data->texture_path.size() >0)
		CreateComponent(ComponentType::MATERIAL, data->texture_path.c_str());

	active = true;
}

//GameObject creator when primitive created
GameObject::GameObject(GameObject* owner, Mesh* data, PrimitiveTypesGL type) {

	data->type = type;
	parent = nullptr;

	//Set GameObject name depending path file info
	if (type == PrimitiveGL_Cube) { name = "Cube_"; name += std::to_string(App->scene->gameobjects.size()); }
	else if (type == PrimitiveGL_Cylinder) { name = "Cylinder_"; name += std::to_string(App->scene->gameobjects.size()); }
	else if (type == PrimitiveGL_Pyramid) { name = "Pyramid_"; name += std::to_string(App->scene->gameobjects.size()); }
	else if (type == PrimitiveGL_Sphere) { name = "Sphere_"; name += std::to_string(App->scene->gameobjects.size()); }

	//Creating always once transform component
	CreateComponent(ComponentType::TRANSFORM);

	//Create Directly Mesh Component
	CreateComponent(ComponentType::MESH, nullptr, data);

	active = true;
}


GameObject::~GameObject() {

	// -- Cleaning components vector
	for (size_t i = 0; i < components.size(); i++) {
		RELEASE(components[i]);
	}

	components.clear();

	// -- Cleaning children vector
	/* for (size_t i = 0; i < children.size(); i++) {
		delete children[i];  ||||| children vector cant be already deleted once again
		because we call all gameObjects delete in Scene cleanUp , so all children will be erased in the process
		just fill with nullptr info and clean vector
		children[i] = nullptr;
	}
	

	children.clear();
	*/

	parent = nullptr;
}

void GameObject::Update(float dt) {

}

//Create Component depending type received less mesh data that will 
Component* GameObject::CreateComponent(ComponentType type, const char* path, Mesh* data) {

	Component* temp = nullptr;

	switch (type)
	{
	case ComponentType::TRANSFORM:
		temp = new ComponentTransform(this);
		break;
	case ComponentType::MATERIAL:
		temp = new ComponentMaterial(this, path);
		break;
	case ComponentType::MESH:
		temp = new ComponentMesh(this, data, path);
		break;
	}

	this->components.push_back(temp);

	return temp;
}


//Search in the components vector the only transform available and returns info
Component* GameObject::GetTransform() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::TRANSFORM)
			return components[i];

	}

	return nullptr;
}

Component* GameObject::GetMesh() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::MESH)
			return components[i];

	}

	return nullptr;
}

Component* GameObject::GetMaterial() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::MATERIAL)
			return components[i];
	}

	return nullptr;
}

void GameObject::SetMaterial(const char* path) {

	ComponentMaterial* mat = new ComponentMaterial(this, path);
	this->components.push_back(mat);
}

void GameObject::DeleteComponent(ComponentType type) {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == type) {
			delete components[i];
			components[i] = nullptr;
		}
	}

	components.shrink_to_fit();
}

string GameObject::SetName(string path) {

	int pos_igual = 0;
	int path_size = path.size() - 4;

	//Erase the .obj, .fbx, etc...
	string new_path = path.erase(path_size);

	//Set the character we want to found
	char buscar = 0x5c;
	int cont = 0;
	for (int i = 0; i <= new_path.size(); i++) {

		if (new_path[i] == buscar) {

			pos_igual = i + 1;
		}

	}

	string name = new_path.substr(pos_igual) + ("_");

	name += std::to_string(App->scene->gameobjects.size());

	return name.c_str();
}