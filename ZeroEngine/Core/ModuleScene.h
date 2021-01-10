#pragma once

// -- Tools
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

#include "PrimitivesGL.h"
#include "GameObject.h"
class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void SaveScene() const;
	void LoadScene(const char* filePath);

	GameObject* CreateGameObject(GameObject* parent = nullptr);
	
public:
	
	string name;
	vector<GameObject*> gameobjects;
	vector<physx::PxRigidBody*> spheres;

	bool draw;
};
