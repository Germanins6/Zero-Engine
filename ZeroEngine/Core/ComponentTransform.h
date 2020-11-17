#pragma once
#include "Globals.h"
#include "Component.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ComponentTransform : public Component {

public:

	ComponentTransform(GameObject* parent);
	~ComponentTransform();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);

	float4x4 GetLocalMatrix();
	float4x4 GetGlobalMatrix();

	float4x4 UpdateLocalMatrix();
	float4x4 UpdateGlobalMatrix();
	float4x4 UpdateGlobalMatrix(float4x4 parentMatrix);
	
	void UpdateNodeTransforms();
	void UpdateNodeChildrenTransform(GameObject* gameObject);

public:
	
	float3 position;
	float3 scale;
	Quat rotation;
	float3 euler;

	float4x4 localMatrix;
	float4x4 globalMatrix;
	float4x4 parentGlobalMatrix;

};