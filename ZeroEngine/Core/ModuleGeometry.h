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

//Forward declaration
struct aiScene;
struct aiNode;

using namespace std;

enum PrimitiveTypesGL;

class ModuleGeometry : public Module
{
public:
	
	ModuleGeometry(Application* app, bool start_enabled = true);
	~ModuleGeometry();

	bool Init();
	update_status Update(float dt);
	bool CleanUp();

	bool LoadGeometry(const char* path);
	GameObject* LoadNodes(const aiScene* scene, aiNode* node, const char* path);

	Mesh* CubeGL();
	Mesh* SphereGL();
	Mesh* PyramidGL();
	Mesh* CylinderGL();

public:

	vector <Mesh*> primitives_storage;
};