#pragma once

// -- Tools
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "ModuleGeometry.h"

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

	GameObject* CreateGameObject(GameObject* parent = nullptr);
	GameObject* CreateGameObject(Mesh* data,const char* path, GameObject* parent = nullptr);
	GameObject* CreateGameObject(PrimitiveTypesGL type, Mesh* data, GameObject* parent = nullptr);
	
public:
	
	vector<GameObject*> rootGameobjects;
	vector<GameObject*> gameobjects;
	bool draw;
};
