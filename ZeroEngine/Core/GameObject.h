#pragma once

// -- Tools
#include <vector>
#include <string>
using namespace std;

// -- Headers
#include "Component.h"

class GameObject {

	friend class Component;

public:

	//name, parent...
	GameObject();
	~GameObject();

	void Update(float dt);
	Component* CreateComponent(ComponentType type);

public:
	string name;
	vector<Component*> components;
	GameObject* parent;
	vector<GameObject*> children;
};