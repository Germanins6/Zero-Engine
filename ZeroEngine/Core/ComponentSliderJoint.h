#pragma once

#include "Component.h"

#include "physx/include/PxPhysicsAPI.h"

class ComponentSliderJoint : public Component {

public:
	ComponentSliderJoint(GameObject* parent);
	~ComponentSliderJoint();

	void CreateJoint(GameObject* draggedGameobject);
	void SetPosition(int actor, physx::PxVec3 position);

	bool Update(float dt);

public:

	physx::PxPrismaticJoint* joint;
	physx::PxRigidDynamic* actorOwner;
	physx::PxRigidDynamic* actorExtern;
	uint32 actorExternReference;

	bool use_limit;

	float upper_limit, lower_limit;
};