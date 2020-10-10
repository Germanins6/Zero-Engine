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
	void CubeGL(vec3 size = 1.0f);
	void CubeDraw(float points[], uint cube_indices[]);

	void SphereGL(uint rings, uint sectors, float radius = 1.0f);
	void SphereDraw(vector<GLfloat> vertices, vector<GLfloat> normals , vector<GLfloat> texcoords, vector<GLushort> indices, vec3 pos = { 0,0,0 });

	void CylinderGL(float radius = 1.0f, float height = 1.0f);
	void CylinderDraw();

	void PyramidGL(uint faces = 3);
	void PyramidDraw();
	
public:


};