#pragma once

#include "Component.h"
#include "Globals.h"
#include "Module.h"
#include "MathGeoLib/include/MathGeoLib.h"

#include "physx/include/PxPhysicsAPI.h"

class GameObject;
class ComponentTransform;
class ComponentRigidDynamic;

class ComponentCollider : public Component {

public:

	ComponentCollider(GameObject* parent);
	~ComponentCollider();
	
	bool Update(float dt);

	void UpdateMatrix();
	inline void UpdateLocalMatrix() { localMatrix = float4x4::FromTRS(colliderPos, colliderRot, colliderSize); };
	inline math::float4x4 GetLocalMatrix() { return localMatrix; };

	inline void UpdateGlobalMatrix() { globalMatrix = ownerMatrix * localMatrix; };
	inline math::float4x4 GetGlobalMatrix() { return globalMatrix; };

	void SetPosition(float3 position);
	inline float3 GetPosition() { return colliderPos; };

	void SetRotation(float3 rotation);
	inline Quat GetRotation() { return colliderRot; };
	inline float3 GetEuler() { return colliderEuler; };

	void SetScale(float3 scale);
	inline float3 GetScale() { return colliderSize; };

public:

	bool isTrigger;

	physx::PxShape* colliderShape;
	physx::PxMaterial* colliderMaterial;
	ComponentTransform* transform;
	ComponentRigidDynamic* rigidbody;

	math::float4x4 globalMatrix = math::float4x4::identity;
	math::float4x4 localMatrix = math::float4x4::identity;

	math::float4x4 ownerMatrix;

	float3 colliderPos, PosOffeset;
	float3 colliderSize, SizeOffset;
	float3 colliderEuler, EulerOffset;
	Quat colliderRot;
	
};