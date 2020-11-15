#include "ComponentTransform.h"

#include "glew/include/glew.h"

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent, ComponentType::TRANSFORM) {
	
	position = { 0.0f, 0.0f, 0.0f };
//	rotation = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f , 1.0f , 1.0f };
}

ComponentTransform::~ComponentTransform() {

}

bool ComponentTransform::Update(float dt) {
	//LOG("Transform update");
	glTranslatef(position.x, position.y, position.z);
	return true;
}

void ComponentTransform::SetPosition() {

}

void ComponentTransform::SetRotation() {

}

void ComponentTransform::SetScale() {

}