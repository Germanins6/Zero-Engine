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

void ComponentTransform::SetPosition(float3 pos) {

	position = pos;
	
	UpdateLocalMatrix();

}

void ComponentTransform::SetRotation(float3 rot) {
	
	euler = float3(rot.x, rot.y, rot.z);
	rotation = Quat::FromEulerXYZ(rot.x * DEGTORAD, rot.y * DEGTORAD, rot.z * DEGTORAD);
	
	UpdateLocalMatrix();
}

void ComponentTransform::SetScale(float3 scaling) {

	scale = scaling;

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
	return globalMatrix.Transposed();
}

float4x4 ComponentTransform::GetLocalMatrix() {
	return localMatrix;
}

float3 ComponentTransform::GetEulerAngles() {
	return euler;
}

void ComponentTransform::UpdateNodeTransforms(){

	//If we change the parent we update his Global matrix and child matrix
	UpdateGlobalMatrix();

	if (owner->children.size() > 0) {

		for (int i = 0; i < owner->children.size(); i++) {
			
			dynamic_cast<ComponentTransform*>(owner->children[i]->GetTransform())->UpdateGlobalMatrix(GetGlobalMatrix().Transposed());
			
			if (owner->children[i]->children.size() > 0) { 
				
				dynamic_cast<ComponentTransform*>(owner->children[i]->GetTransform())->UpdateNodeTransforms();

			}
		}

	}

}

void ComponentTransform::SetTransformMatrix(float4x4 new_transform_matrix) {

	globalMatrix = new_transform_matrix;

	float4x4 new_parent_inverse = parentGlobalMatrix;
	new_parent_inverse.Inverse();

	localMatrix = new_parent_inverse * globalMatrix;
	localMatrix.Decompose(position, rotation, scale);

	UpdateNodeTransforms();
}