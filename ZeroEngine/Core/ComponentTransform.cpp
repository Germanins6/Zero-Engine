#include "ComponentTransform.h"

#include "glew/include/glew.h"

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent, ComponentType::TRANSFORM) {
	
	position = float3::zero;
	scale = float3::one;
	euler = float3::zero;
	rotation = Quat::identity;

	localMatrix = float4x4::identity;
	globalMatrix = float4x4::identity;
	parentGlobalMatrix = float4x4::identity;
}

ComponentTransform::~ComponentTransform() {

}

void ComponentTransform::SetPosition(float x, float y, float z) {

	position.x = x;
	position.y = y;
	position.z = z;
	
	UpdateLocalMatrix();

}

void ComponentTransform::SetRotation(float x, float y, float z) {

	euler = float3(x, y, z);
	rotation = Quat::FromEulerXYZ(x * DEGTORAD, y * DEGTORAD, z * DEGTORAD);

	UpdateLocalMatrix();
}

void ComponentTransform::SetScale(float x, float y, float z) {

	scale.x = x;
	scale.y = y;
	scale.z = z;

	UpdateLocalMatrix();
}

float4x4 ComponentTransform::UpdateGlobalMatrix(float4x4 parentMatrix) {

	UpdateLocalMatrix();
	parentGlobalMatrix = parentMatrix;
	globalMatrix = parentGlobalMatrix * localMatrix;

	return globalMatrix;
}

float4x4 ComponentTransform::UpdateGlobalMatrix() {

	UpdateLocalMatrix();
	globalMatrix = parentGlobalMatrix * localMatrix;

	return globalMatrix;
}

float4x4 ComponentTransform::UpdateLocalMatrix() {

	localMatrix = float4x4::FromTRS(position, rotation, scale);
	return localMatrix;
}

float4x4 ComponentTransform::GetGlobalMatrix() {
	return globalMatrix;
}

float4x4 ComponentTransform::GetLocalMatrix() {
	return localMatrix;
}

void ComponentTransform::UpdateNodeTransforms(){

	//If we change the parent we update his Global matrix and child matrix
	UpdateGlobalMatrix();

	if (owner->children.size() > 0) {

		for (int i = 0; i < owner->children.size(); i++) {
			dynamic_cast<ComponentTransform*>(owner->children[i]->GetTransform())->UpdateGlobalMatrix(GetGlobalMatrix());
			if (owner->children[i]->children.size() > 0) { UpdateNodeChildrenTransform(owner->children[i]); }
		}

	}

}

void ComponentTransform::UpdateNodeChildrenTransform(GameObject* gameObject) {

	for (size_t i = 0; i < gameObject->children.size(); i++)
	{
			dynamic_cast<ComponentTransform*>(gameObject->children[i]->GetTransform())->UpdateGlobalMatrix(GetGlobalMatrix());
			UpdateNodeChildrenTransform(gameObject->children[i]);
	}

}
