#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#include "glew/include/glew.h"

using namespace std;

class Primitives : public Module
{
public:
	Primitives(Application* app, bool start_enabled = true);
	~Primitives();

	bool Init();
	bool CleanUp();

	//Primitives
	void CubeGL(vec3 size = 1.0f, vec3 pos = { 0,0,0 });
	void CubeDraw(float points[], uint cube_indices[], vec3 pos = { 0,0,0 });

	void SphereGL(uint rings, uint sectors, float radius = 1.0f, vec3 pos = { 0,0,0 });
	void SphereDraw(vector<GLfloat> vertices, vector<GLushort> indices, vec3 pos = { 0,0,0 });

	void CylinderGL(float slice = 30.f, int rings = 20, float radius = 1.0f, float height = 1.0f);
	void CylinderDraw(vector<float> vertices, vector<uint> indices);

	void PyramidGL(uint faces = 3, vec3 size = 1.0f, vec3 pos = { 0,0,0 });
	void PyramidDraw(float points[], uint indices[], vec3 pos = { 0,0,0 });
	
public:


};