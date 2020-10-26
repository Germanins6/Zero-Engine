#pragma once

// -- Tools
#include <vector>
#include <string>
using namespace std;

// -- Headers
#include "Component.h"
#include "ComponentMesh.h" // From 0 to N
#include "ComponentTransform.h"	// 1 Each GO
#include "ComponentMaterial.h"	// From 0 to 1

#include "PrimitivesGL.h"

class GameObject {

	friend class Component;

public:

	//name, parent...
	GameObject();
	//GameObject(GameObject* parent);
	GameObject(GameObject* parent, Mesh* data, const char* path);
	GameObject(GameObject* parent, PrimitiveTypesGL type);
	~GameObject();

	void Update(float dt);
	Component* CreateComponent(ComponentType type);
	Component* CreateComponent(Mesh* data ,const char* path);

	// -- Tools to access component info from gameObject pointer
	Component* GetTransform();
	Component* GetMesh();
	Component* GetMaterial();

	string SetName(string path);

public:
	string name;
	GameObject* parent;
	vector<GameObject*> children;
	vector<Component*> components;

};