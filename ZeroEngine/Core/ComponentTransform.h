#pragma once

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

#include "Globals.h"

#include "ComponentMesh.h"
#include "Assimp/include/vector3.h"
#include "Assimp/include/quaternion.h"

struct TransformData {

	TransformData(float3 pos, float3 eu, float3 scale, Quat rot) : position{ pos }, euler{ eu }, scaling{ scale }, rotation{ rot } {}
	~TransformData() {}
	float3 position, euler, scaling;
	Quat rotation;

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);

	float4x4 GetLocalTransform();
	float4x4 GetGlobalTransform();

	float4x4 local_transform = float4x4::identity;
	float4x4 global_transform = float4x4::identity;
};

class ComponentTransform : public Component {

public:

	ComponentTransform(GameObject* parent, Mesh* data);
	~ComponentTransform();

	bool Update(float dt);

	TransformData* GetTransform();
	

public:

	float4x4 localMatrix;
	float4x4 globalMatrix;


	TransformData* info;

};