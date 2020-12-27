#pragma once

#include "Component.h"
#include "Globals.h"
#include "Module.h"

class GameObject;

class ComponentRigidDynamic : public Component {

public:

	ComponentRigidDynamic(GameObject* parent);
	~ComponentRigidDynamic();

	bool Update(float dt);

public:

};