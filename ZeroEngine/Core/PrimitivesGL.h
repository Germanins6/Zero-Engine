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


class Primitives
{
public:
	Primitives();
	~Primitives();

	bool Init();

	void AxisGL(int size = 10);

public:

	uint my_vertex, my_indices;
	vec3 pos, size;
	vec4 rot;

};