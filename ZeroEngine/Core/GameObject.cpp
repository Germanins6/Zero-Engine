#include "GameObject.h"

// -- Components
#include "ComponentTransform.h"

GameObject::GameObject() {
	name = "HOLA";
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