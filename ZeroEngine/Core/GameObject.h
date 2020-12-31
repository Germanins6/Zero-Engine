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
#include "ComponentRigidDynamic.h"
#include "ComponentCollider.h"
#include "ComponentDistanceJoint.h"
#include "ComponentRevoluteJoint.h"

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
	ComponentTransform* GetTransform();
	ComponentMesh* GetMesh();
	ComponentMaterial* GetMaterial();
	ComponentCamera* GetCamera();
	ComponentRigidDynamic* GetRigidbody();
	ComponentCollider* GetCollider();
	ComponentDistanceJoint* GetDistanceJoint();
	ComponentRevoluteJoint* GetRevoluteJoint();

	void ReParent(GameObject* child, GameObject* new_parent);

	//BoundingBox
	inline math::AABB GetAABB() const { return bbox; };
	inline math::OBB GetOBB() const { return obb; };
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
	math::OBB obb;

};