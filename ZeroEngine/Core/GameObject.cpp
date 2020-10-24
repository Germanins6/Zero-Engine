#include "GameObject.h"

// -- Components
#include "ComponentTransform.h"

GameObject::GameObject() {
	
	name = "Empty GameObject";
	
}

GameObject::GameObject(GameObject* parent, Mesh* data, const char* path) {
	
	CreateComponent(ComponentType::MESH);

	if (strlen(path) > 0) {
		name = SetName(path);
	}
	else {
		name = "Empty GameObject";
	}

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

Component* GameObject::CreateComponent(ComponentType type) {

	Component* temp = nullptr;

	switch (type)
	{
	case ComponentType::TRANSFORM:
		temp = new ComponentTransform(this);
		break;
	case ComponentType::MESH:
		temp = new ComponentMesh(this);
		break;
	case ComponentType::MATERIAL:
		break;
	case ComponentType::PARTICLE:
		break;
	case ComponentType::SCRIPT:
		break;
	}

	components.push_back(temp);

	return temp;
}

std::string GameObject::SetName(std::string path) {

	int pos_igual = 0;
	int path_size = path.size() - 4;

	//Erase the .obj, .fbx, etc...
	std::string new_path = path.erase(path_size);

	//Set the character we want to found
	char buscar = '/';

	for (int i = new_path.size(); i >= 0; i--) {
		if (new_path[i] == buscar) {
			pos_igual = i;
		}
	}

	std::string name = new_path.substr(pos_igual + 1);

	return name.c_str();
}