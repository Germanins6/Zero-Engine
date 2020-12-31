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

	physx::PxDistanceJoint* joint;
	physx::PxRigidDynamic* actorOwner;
	physx::PxRigidDynamic* actorExtern;
};