#pragma once

#include "Component.h"

#include "physx/include/PxPhysicsAPI.h"

class ComponentRevoluteJoint : public Component {

public:
	ComponentRevoluteJoint(GameObject* parent);
	~ComponentRevoluteJoint();

	void CreateJoint(GameObject* draggedGameobject);
	void SetPosition(int actor, physx::PxVec3 position);

public:

	physx::PxRevoluteJoint* joint;
	physx::PxRigidDynamic* actorOwner;
	physx::PxRigidDynamic* actorExtern;

	bool use_limit = false;
	bool use_motor = false;
	bool use_freespin = false;

	float velocity = 10.0f;
	float upper_limit, lower_limit;
};