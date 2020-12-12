#pragma once

#include "p2Defs.h"

// -- Tools
#include <vector>
#include <string>
using namespace std;

// -- Headers
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h" 

class GameObject {

	friend class Component;

public:

	//name, parent...
	GameObject();
	~GameObject();

	void Update(float dt);
	Component* CreateComponent(ComponentType type, Resource* ourResource = nullptr);

	//Gameobject info getters
	inline uint32 Getuid() { return uuid; }

	// -- Tools to access component info from gameObject pointer
	Component* GetTransform();
	Component* GetMesh();
	Component* GetMaterial();
	Component* GetCamera();

	void ReParent(GameObject* child, GameObject* new_parent);

	//BoundingBox
	inline math::AABB GetAABB() const { return bbox; };
	void UpdateBB();
	void DrawAABB();

public:

	uint32 uuid;
	uint32 parentId;
	string name;
	GameObject* parent;
	vector<GameObject*> children;
	vector<Component*> components;
	
	bool active;
	bool draw_boundingBox;

	math::AABB bbox;

};