#pragma once

#include "Component.h"

#include "physx/include/PxPhysicsAPI.h"

class ComponentDistanceJoint : public Component {

public:
	ComponentDistanceJoint(GameObject* parent);
	~ComponentDistanceJoint();

	void CreateJoint(GameObject* draggedGameobject);
	void SetPosition(int actor, physx::PxVec3 position);

public:

	physx::PxDistanceJoint* joint;
	physx::PxRigidDynamic* actorOwner;
	physx::PxRigidDynamic* actorExtern;
};