#pragma once

#include "Component.h"
#include "Globals.h"
#include "Module.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "ModulePhysics.h"

#include "physx/include/PxPhysicsAPI.h"

class GameObject;
class ComponentTransform;
class ComponentRigidDynamic;

enum class GeometryType;

class ComponentCollider : public Component {

public:

	ComponentCollider(GameObject* parent);
	~ComponentCollider();
	
	bool Update(float dt);

	void SetPosition(float3 position);
	inline float3 GetPosition() { return colliderPos; };

	void SetRotation(float3 rotation);
	inline float3 GetEuler() { return colliderEuler; };

	void SetScale(float3 scale);
	inline float3 GetScale() { return colliderSize; };

	void UpdateValues();

public:

	bool isTrigger;

	physx::PxShape* colliderShape;
	physx::PxMaterial* colliderMaterial;
	ComponentTransform* transform;
	ComponentRigidDynamic* rigidbody;

	float3 colliderPos = float3::zero;
	float3 colliderSize = float3::one;
	float3 colliderEuler = float3::zero;

	float3 colliderDim = float3::one;
	
	GeometryType type = GeometryType::NONE;

};