#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include <vector>
#include "glew/include/glew.h"

using namespace std;
#define SIZE_INITIAL 5
#define PI 3.1415

enum PrimitiveTypesGL
{
	PrimitiveGL_NONE,
	PrimitiveGL_Cube,
	PrimitiveGL_Sphere,
	PrimitiveGL_Triangle_pyramid,
	PrimitiveGL_Pyramid,
	PrimitiveGL_Cylinder
};

class Primitives
{
public:
	Primitives();
	~Primitives();

	bool Init();

	void AxisGL(int size = 10);

public:

	uint my_vertex, my_indices;
	PrimitiveTypesGL type;
	vec3 pos, size;
	vec4 rot;

	int indices_amount;
	int vertices_amount;
	float* vertices_;
	short* indices_;
};

// ============================================
class CubeGL : public Primitives
{
public:
	CubeGL(vec3 size = {1.0f, 1.0f, 1.0f});
	~CubeGL();
	void InnerRender(vec3 pos = { 0.0f, 0.0f, 0.0f }, vec4 rotation = {0, 0.0f, 0.0f , 0.0f}) const;
};

// ============================================
class SphereGL : public Primitives
{
public:
	SphereGL(uint rings = 20, uint sectors = 20, float radius = 1.0f);
	~SphereGL();
	void InnerRender(vec3 pos = { 0.0f, 0.0f, 0.0f }, vec4 rotation = { 0, 0.0f, 0.0f , 0.0f }) const;

};

// ============================================
class PyramidGL : public Primitives
{
public:
	PyramidGL(uint faces = 4, vec3 size = { 1.f, 1.f, 1.f }, float height = 1.f, float face_lenght = 1.f);
	~PyramidGL();
	void InnerRender(vec3 pos = { 0.0f, 0.0f, 0.0f }, vec4 rotation = { 0, 0.0f, 0.0f , 0.0f }) const;

public:
	vector<vec3> points;
	vector<int> indices;
};

// ============================================
class CylinderGL : public Primitives
{
public:
	CylinderGL(float slice = 30.f, float radius = 1.0f, float height = 1.0f);
	~CylinderGL();
	void InnerRender(vec3 pos = { 0.0f, 0.0f, 0.0f }, vec4 rotation = { 0, 0.0f, 0.0f , 0.0f }) const;
};