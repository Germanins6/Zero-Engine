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

enum PrimitiveTypesGL;

class GameObject {

	friend class Component;

public:

	//name, parent...
	GameObject();
	GameObject(GameObject* parent, Mesh* data, PrimitiveTypesGL type);
	~GameObject();

	void Update(float dt);
	Component* CreateComponent(ComponentType type, const char* path = nullptr, Mesh* data = nullptr);

	//Gameobject info getters
	inline uint32 Getuid() { return uuid; }

	// -- Tools to access component info from gameObject pointer
	Component* GetTransform();
	Component* GetMesh();
	Component* GetMaterial();

	void SetMaterial(const char* path);

	void DeleteComponent(ComponentType type);
	void ReParent(GameObject* child, GameObject* new_parent);

	string SetName(string path);

public:

	uint32 uuid;
	uint32 parentId;
	string name;
	GameObject* parent;
	vector<GameObject*> children;
	vector<Component*> components;
	
	bool active;

};