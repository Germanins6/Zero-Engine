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

	uint id_normals = 0;
	uint num_normals = 0;
	float* normals = nullptr;
	std::vector<vec3> normal_face_vector_direction = { 0,0,0 };
	std::vector<vec3> normal_faces = { 0,0,0 };

	uint my_vertex = 0;
	uint my_indices = 0;
	uint my_normals = 0;

	void GenerateBufferGeometry();
	void RenderGeometry();

	bool renderVertexNormals = true;
	bool renderFaceNormals = true;
};

class ModuleGeometry : public Module
{
public:
	
	ModuleGeometry(Application* app, bool start_enabled = true);
	~ModuleGeometry();

	bool Init();
	update_status Update(float dt);
	bool CleanUp();

	bool LoadGeometry(Mesh* mesh, const char* path);

public:
	std::vector <Mesh*> geometry_storage;
	Mesh* geometry_data;

};