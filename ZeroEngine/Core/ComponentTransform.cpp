#include "ComponentTransform.h"
#include "glew/include/glew.h"

ComponentTransform::ComponentTransform(GameObject* parent, Mesh* data) : Component(parent, ComponentType::TRANSFORM) {
	
	info = new TransformData({ 0,0,0 }, { 0,0,0 }, { 1,1,1 }, {0,0,0,0});

	if (data != nullptr) {
		data->resource_mesh->transform = info;
	}
	
}


ComponentTransform::~ComponentTransform() {

}

bool ComponentTransform::Update(float dt) {
	//LOG("Transform update");
	
	return true;
}

void TransformData::SetPosition(float x, float y, float z) {
	
	position.x = x;
	position.y = y;
	position.z = z;

}

void TransformData::SetRotation(float x, float y, float z) {
		
	euler = float3(x, y, z);
	rotation = Quat::FromEulerXYZ(x * DEGTORAD, y * DEGTORAD, z * DEGTORAD);

}

void TransformData::SetScale(float x, float y, float z) {
	
	scaling.x = x;
	scaling.y = y;
	scaling.z = z;

}

TransformData* ComponentTransform::GetTransform() {

	return info;

}

float4x4 TransformData::GetLocalTransform() {

	local_transform = float4x4::FromTRS(position, rotation, scaling);

	return local_transform;
}

float4x4 TransformData::GetGlobalTransform() {

	global_transform = local_transform;

	return global_transform;
}