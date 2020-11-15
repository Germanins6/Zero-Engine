#include "ComponentTransform.h"

#include "glew/include/glew.h"

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent, ComponentType::TRANSFORM) {
	
	position = { 0.0f, 0.0f, 0.0f };
	rotation = { 0.0f, 0.0f, 0.0f , 0.0f};
	scale = { 1.0f , 1.0f , 1.0f };
	euler = { 0.0f, 0.0f, 0.0f };

	localMatrix = float4x4::identity;
	globalMatrix = float4x4::identity;
}

ComponentTransform::~ComponentTransform() {

}

bool ComponentTransform::Update(float dt) {
	//glTranslatef(position.x, position.y, position.z);
	return true;
}

void ComponentTransform::SetPosition(float x, float y, float z) {

	position.x = x;
	position.y = y;
	position.z = z;

}

void ComponentTransform::SetRotation(float x, float y, float z) {

	euler = float3(x, y, z);
	rotation = Quat::FromEulerXYZ(x * DEGTORAD, y * DEGTORAD, z * DEGTORAD);
}

void ComponentTransform::SetScale(float x, float y, float z) {

	scale.x = x;
	scale.y = y;
	scale.z = z;
}

float4x4 ComponentTransform::GetLocalMatrix() {

	localMatrix = float4x4::FromTRS(position, rotation, scale);
	return localMatrix;
}

float4x4 ComponentTransform::GetGlobalMatrix() {
	return globalMatrix;
}