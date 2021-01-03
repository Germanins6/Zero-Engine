#pragma once

// -- Tools
#include "glmath.h"
#include <vector>
#include <string>
using namespace std;

//Forward declaration
class GameObject;

enum class ComponentType {
	TRANSFORM,
	MESH,
	MATERIAL,
	CAMERA,
	RIGIDBODY,
	COLLIDER,
	DISTANCE_JOINT,
	REVOLUTE_JOINT,
	PRISMATIC_JOINT,
	SPHERICAL_JOINT,
	MAX_COMPONENTS,

};

class Component {

public:

	Component(GameObject* parent, ComponentType ty);
	virtual ~Component();

	virtual bool Update(float dt) {
		return true;
	}

	virtual bool Enable() { return active = true; };
	virtual bool Disable() { return active = false; };

public:
	ComponentType type;
	GameObject* owner;
	bool active;
};