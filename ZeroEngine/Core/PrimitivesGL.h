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

	int indices_amount;
	int vertices_amount;
	float* vertices_;
	short* indices_;
};