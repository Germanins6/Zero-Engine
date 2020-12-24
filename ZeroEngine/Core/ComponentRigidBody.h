#pragma once
#include "Component.h"

//Tools
#include "Globals.h"
#include "ModulePhysics.h"

struct Constrain {

	Constrain(bool _x = false, bool _y = false, bool _z = false) : x{_x}, y{_y}, z{_z}{}
	bool x, y, z;
};

class ComponentRigidBody : public Component {

public:

	ComponentRigidBody(GameObject* parent);
	~ComponentRigidBody();

public:

	physx::PxRigidDynamic* rigidbody;

	float mass;
	float drag;
	float angular_drag;
	bool enableGravity;
	bool isKinematic;

	Constrain FreezePosition;
	Constrain FreezeRotation;
};