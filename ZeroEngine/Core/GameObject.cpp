#include "GameObject.h"

// -- Components
#include "ComponentTransform.h"

GameObject::GameObject() {
	name = "Empty GameObject";
}

GameObject::GameObject(GameObject* parent, Mesh* data, const char* path) {
	CreateComponent(ComponentType::MESH);
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