#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#include "glew/include/glew.h"

using namespace std;
#define SIZE_INITIAL 5

class Primitives : public Module
{
public:
	Primitives(Application* app, bool start_enabled = true);
	~Primitives();

	bool Init();
	bool CleanUp();
	int i = 0;
	vec3 vertice;
	//Primitives
	void CubeGL(vec3 size = 1.0f, vec3 pos = { 0,0,0 });
	void CubeDraw(float points[], uint cube_indices[], vec3 pos = { 0,0,0 });

	void SphereGL(uint rings, uint sectors, float radius = 1.0f, vec3 pos = { 0,0,0 });
	void SphereDraw(float vertices[], short indices[], int vertices_amount, vec3 pos = { 0,0,0 });

	void CylinderGL(float slice = 30.f, int rings = 20, float radius = 1.0f, float height = 1.0f);
	void CylinderDraw(vector<float> vertices, vector<uint> indices);

	void PyramidGL(uint faces = 4, vec3 size = { 1.f, 1.f, 1.f }, vec3 pos = { 0,0,0 }, float height = 1.f, float face_lenght = 1.f);
	void PyramidDraw(vector<vec3> points, vector<int> indices, vec3 pos = { 0,0,0 });
	
	void AxisGL(int size = 10);

public:

	int indices_amount;
};