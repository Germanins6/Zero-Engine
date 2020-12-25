#pragma once

#include "Component.h"
#include "Globals.h"
#include "Module.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/Frustum.h"

class GameObject;

class ComponentCamera : public Component {

public:

	ComponentCamera(GameObject* parent);
	~ComponentCamera();
	
	bool Update(float dt);

	math::float4x4 ViewMatrix();
	math::float4x4 ProjectionMatrix();

	float GetAspectRatio();
	void SetAspectRatio(float aspect_ratio);

	float GetNearDistance();
	void SetNearDistance(float near_distance);

	float GetFarDistance();
	void SetFarDistance(float far_distance);

	float GetFOV();
	void SetFOV(float near_distance);

	void Draw();

	bool Cull(math::AABB bbox);
	void CameraCullGameObjects();

	void LookAt(const math::float3& Spot);

	void SetPos(math::float3 pos);
	void SetRotation();
	void SetReference(math::float3 reference_);
	void SetViewMatrix(math::float4x4 viewMatrix);

public:

	math::Frustum frustum;
	float camera_aspect_ratio = 0.0f;
	bool cull = false;
	math::float3 new_reference;
	bool draw = true;

};