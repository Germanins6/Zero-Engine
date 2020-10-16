#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include <string.h>
#include "glew/include/glew.h"

#define MAX_LIGHTS 8

struct Mesh {
	
	uint id_index; //index in VRAM
	uint num_index;
	uint* index;

	uint id_vertex; //unique vertex in VRAM
	uint num_vertex;
	float* vertex;

};

class ModuleGeometry : public Module
{
public:
	ModuleGeometry(Application* app, bool start_enabled = true);
	~ModuleGeometry();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	void LoadGeometry();
	void DrawGeometry(float vertex[], uint index[]);

public:
	Mesh mesh;
private:
	
};