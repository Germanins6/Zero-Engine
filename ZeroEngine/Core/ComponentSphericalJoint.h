#pragma once

#include "Component.h"

#include "physx/include/PxPhysicsAPI.h"

class ComponentSphericalJoint : public Component {

public:
	ComponentSphericalJoint(GameObject* parent);
	~ComponentSphericalJoint();

	void CreateJoint(GameObject* draggedGameobject);
	void SetPosition(int actor, physx::PxVec3 position);

public:

	physx::PxSphericalJoint* joint;
	physx::PxRigidDynamic* actorOwner;
	physx::PxRigidDynamic* actorExtern;
};