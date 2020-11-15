#pragma once

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ComponentTransform : public Component {

public:

	ComponentTransform(GameObject* parent);
	~ComponentTransform();

	bool Update(float dt);

	void SetPosition();
	void SetRotation();
	void SetScale();

public:
	
	vec3 position;
	Quat rotation;
	float3 scale;

	float4x4 localMatrix;
	float4x4 globalMatrix;
};