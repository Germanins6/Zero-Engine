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
	
	
	void AxisGL(int size = 10);

public:
	int i = 0;
	vec3 vertice;

};


// ============================================
class CubeGL
{
public:
	CubeGL(vec3 size = { 1.f,1.f, 1.f });
	~CubeGL();
	void CubeBuffers();
	void CubeDraw(vec3 pos = { 0,0,0 });


public:
	float points_cube[24];
	uint cube_indices[36];
	uint my_vertex = 0, my_indices = 0;
};

// ============================================
class PyramidGL
{
public:

	void Pyramid(uint faces = 4, vec3 size = { 1.f, 1.f, 1.f }, vec3 pos = { 0,0,0 }, float height = 1.f, float face_lenght = 1.f);
	void PyramidBuffers();
	void PyramidDraw(vec3 pos = { 0,0,0 });


public:
	int indices_amount;
	int vertices_amount;
	float* vertices_;
	short* indices_;

	uint my_vertex = 0, my_indices = 0;
};

// ============================================
class SphereGL
{
public:
	void Sphere(uint rings, uint sectors, float radius = 1.0f);
	void SphereBuffers();
	void SphereDraw(vec3 pos = { 0,0,0 });
public:
	int indices_amount;
	int vertices_amount;
	float* vertices_;
	short* indices_;

	uint my_vertex = 0, my_indices = 0;
};

// ============================================
class CylinderGL
{
public:
	void Cylinder(float slice = 30.f, int rings = 20, float radius = 1.0f, float height = 1.0f);
	void CylinderBuffers();
	void CylinderDraw(vec3 pos = { 0,0,0 });
	
public:
	int indices_amount;
	int vertices_amount;
	float* vertices_;
	short* indices_;

	uint my_vertex = 0, my_indices = 0;
};

// ============================================
