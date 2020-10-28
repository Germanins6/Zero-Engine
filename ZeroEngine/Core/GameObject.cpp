#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"

//Empty GameObject constructor
GameObject::GameObject() {

	name = name + ("EmptyObject_");

	name += std::to_string(App->scene->rootGameobjects.size());

	CreateComponent(ComponentType::TRANSFORM);
}

//GameObject creator when mesh loaded
GameObject::GameObject(GameObject* parent, Mesh* data, const char* path) {

	//Set GameObject name depending path file info
	if (strlen(path) > 0) name = SetName(path);
	else name = "Empty GameObject";

	if (data->num_meshes > 0) {
		children.push_back(parent);
	}

	//Creating always once transform component
	CreateComponent(ComponentType::TRANSFORM);

	//Create Directly Mesh Component
	CreateComponent(data, path);

}

//GameObject creator when primitive created
GameObject::GameObject(GameObject* parent, Mesh* data, PrimitiveTypesGL type) {

	data->type = type;
	//Set GameObject name depending path file info
	if (type == PrimitiveGL_Cube) { name = "Cube_"; name += std::to_string(App->scene->gameobjects.size()); }
	else if (type == PrimitiveGL_Cylinder) { name = "Cylinder_"; name += std::to_string(App->scene->gameobjects.size()); }
	else if (type == PrimitiveGL_Pyramid) { name = "Pyramid_"; name += std::to_string(App->scene->gameobjects.size()); }
	else if (type == PrimitiveGL_Sphere) { name = "Sphere_"; name += std::to_string(App->scene->gameobjects.size()); }


	//Creating always once transform component
	CreateComponent(ComponentType::TRANSFORM);

	//Create Directly Mesh Component
	CreateComponent(data);

}


GameObject::~GameObject() {

	// -- Cleaning components vector
	for (size_t i = 0; i < components.size(); i++)
		components[i] = nullptr;

	components.clear();

	// -- Cleaning children vector
	for (size_t i = 0; i < children.size(); i++)
		children[i] = nullptr;

	children.clear();

	parent = nullptr;
}

void GameObject::Update(float dt) {

}

//Create Component depending type received less mesh data that will 
Component* GameObject::CreateComponent(ComponentType type) {

	Component* temp = nullptr;

	switch (type)
	{
	case ComponentType::TRANSFORM:
		temp = new ComponentTransform(this);
		break;
	case ComponentType::MATERIAL:
		temp = new ComponentMaterial(this, App->input->file_path);
		break;
	}

	this->components.push_back(temp);

	return temp;
}


//Overload to just create directly a component if a mesh info received into GameObject consctructor
Component* GameObject::CreateComponent(Mesh* data, const char* path) {

	Component* temp = new ComponentMesh(this, data, path);
	this->components.push_back(temp);
	return temp;
}

//Overload to just create directly a component if a mesh info received into GameObject consctructor
Component* GameObject::CreateComponent(Mesh* data) {

	Component* temp = new ComponentMesh(this, data);
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

void GameObject::DeleteComponent(ComponentType type) {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == type) {
			components[i]->~Component();
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