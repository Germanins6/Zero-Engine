#pragma once

#include "Component.h"
#include "Globals.h"
#include "Module.h"

#include "physx/include/PxPhysicsAPI.h"

class GameObject;

class ComponentCollider : public Component {

public:

	ComponentCollider(GameObject* parent);
	~ComponentCollider();
	
	bool Update(float dt);

public:

	bool drawCollider = true;

	physx::PxShape* colliderShape;

};