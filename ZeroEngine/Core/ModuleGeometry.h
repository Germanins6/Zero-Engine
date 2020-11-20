#pragma once
#include "Module.h"

// -- Tools
#include "PrimitivesGL.h"
#include "glmath.h"

using namespace std;

enum PrimitiveTypesGL;

class Mesh;

class ModuleGeometry : public Module
{
public:
	
	ModuleGeometry(Application* app, bool start_enabled = true);
	~ModuleGeometry();

	Mesh* CubeGL();
	Mesh* SphereGL();
	Mesh* PyramidGL();
	Mesh* CylinderGL();

public:

	vector <Mesh*> primitives_storage;
};