#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include <string.h>
#include "glew/include/glew.h"

#define MAX_LIGHTS 8

struct Mesh {
	
	uint id_index = 0; //index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint id_vertex = 0; //unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

	void GenerateBufferGeometry(Mesh* mesh);
	void RenderGeometry(Mesh* mesh);
	
	uint my_vertex = 0;
	uint my_indices = 0;

};

class ModuleGeometry : public Module
{
public:
	
	ModuleGeometry(Application* app, bool start_enabled = true);
	~ModuleGeometry();

	bool Init();
	bool CleanUp();

	bool LoadGeometry(Mesh* mesh, const char* path);

public:
	


private:

};