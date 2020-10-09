#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#include "glew/include/glew.h"

class Primitives : public Module
{
public:
	Primitives(Application* app, bool start_enabled = true);
	~Primitives();

	bool Init();
	bool CleanUp();
	bool CubeGL(vec3 size = 1.f);
	bool CubeDraw(float points[], uint cube_indices[]);
	
public:


};