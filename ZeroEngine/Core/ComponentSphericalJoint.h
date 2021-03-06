#pragma once

#include "Component.h"

#include "physx/include/PxPhysicsAPI.h"

class ComponentSphericalJoint : public Component {

public:
	ComponentSphericalJoint(GameObject* parent);
	~ComponentSphericalJoint();

	void CreateJoint(GameObject* draggedGameobject);
	void SetPosition(int actor, physx::PxVec3 position);

	bool Update(float dt);

public:

	physx::PxSphericalJoint* joint;
	physx::PxRigidDynamic* actorOwner;
	physx::PxRigidDynamic* actorExtern;
	uint32 actorExternReference;

	bool use_limit = false;

	float lower_limit, upper_limit;
};