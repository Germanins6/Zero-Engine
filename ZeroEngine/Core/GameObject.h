#pragma once

// -- Tools
#include <vector>
#include <string>
using namespace std;

// -- Headers
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

class GameObject {

	friend class Component;

public:

	//name, parent...
	GameObject();
	//GameObject(GameObject* parent);
	GameObject(GameObject* parent, const char* path);
	~GameObject();

	void Update(float dt);
	Component* CreateComponent(ComponentType type);
	Component* CreateComponent(const char* path);

	string SetName(string path);

public:
	string name;
	vector<Component*> components;
	GameObject* parent;
	vector<GameObject*> children;

};