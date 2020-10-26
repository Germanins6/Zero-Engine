#pragma once
#include "Module.h"

// -- Tools
#include "PrimitivesGL.h"
#include "Light.h"
#include "glmath.h"
#include <string.h>

//Forward declaration from ComponentMesh;
#include "ComponentMesh.h"

#define MAX_LIGHTS 8
#define CHECKERS_HEIGHT 32
#define CHECKERS_WIDTH 32

using namespace std;

class ModuleGeometry : public Module
{
public:
	
	ModuleGeometry(Application* app, bool start_enabled = true);
	~ModuleGeometry();

	bool Init();
	update_status Update(float dt);
	bool CleanUp();

	Mesh* LoadGeometry(const char* path);

public:

	vector <Mesh*> geometry_storage;
	vector <Primitives*> primitives_storage;

	Mesh* geometry_data;
	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];

};