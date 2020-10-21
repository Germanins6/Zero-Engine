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

protected:
	PrimitiveTypesGL type;
	vec3 pos, size;

	int indices_amount;
	int vertices_amount;
	float* vertices_;
	short* indices_;
};

// ============================================
class CubeGL : public Primitives
{
public:
	CubeGL();
	CubeGL(vec3 size);
	void InnerRender(vec3 pos, vec4 rotation) const;
};

// ============================================
class SphereGL : public Primitives
{
public:
	SphereGL();
	SphereGL(uint rings, uint sectors, float radius = 1.0f);
	void InnerRender(vec3 pos, vec4 rotation) const;
public:
	int indices_amount;
	int vertices_amount;
	float* vertices_;
	short* indices_;
};

// ============================================
class PyramidGL : public Primitives
{
public:
	PyramidGL();
	PyramidGL(uint faces = 4, vec3 size = { 1.f, 1.f, 1.f }, float height = 1.f, float face_lenght = 1.f);
	void InnerRender(vec3 pos, vec4 rot) const;
public:
	vec3 size;
	vector<vec3> points;
	vector<int> indices;
	int indices_amount;
	int vertices_amount;
	float* vertices_;
	short* indices_;
};

// ============================================
class CylinderGL : public Primitives
{
public:
	CylinderGL();
	CylinderGL(float slice = 30.f, float radius = 1.0f, float height = 1.0f);
	void InnerRender(vec3 pos, vec4 rot) const;
public:
	int indices_amount;
	int vertices_amount;
	float* vertices_;
	short* indices_;
};