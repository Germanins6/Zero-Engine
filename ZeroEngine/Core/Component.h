#pragma once

// -- Tools
#include <vector>
#include <string>
using namespace std;

//Forward declaration
class GameObject;

enum class ComponentType {
	TRANSFORM,
	MESH,
	MATERIAL,
	PARTICLE,
	SCRIPT,
	MAX_COMPONENTS,
};

class Component {

public:

	Component(GameObject* parent, ComponentType ty);
	~Component();

	virtual bool Update(float dt);

	virtual bool Enable() {};
	virtual bool Disable() {};

public:
	ComponentType type;
	GameObject* owner;
	bool active;
};