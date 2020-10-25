#pragma once
#include "Component.h"

// -- Tools
#include "Globals.h"

#include "PrimitivesGL.h"
#include "Light.h"
#include "glmath.h"
#include <string.h>

// -- ModuleTextures
#include "ModuleTextures.h"

struct Mesh {

	void GenerateBufferGeometry();
	void RenderGeometry();

	uint id_index = 0; //index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint id_vertex = 0; //unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

	uint id_normals = 0;
	uint num_normals = 0;
	uint num_normal_faces = 0;

	float* normals = nullptr;
	float* normal_face_vector_direction = nullptr;
	float* normal_faces = nullptr;

	uint my_vertex = 0;
	uint my_indices = 0;
	uint my_normals = 0;
	uint my_texture = 0;

	uint textureID = 0;
	float* uv_coords = nullptr;

	bool renderVertexNormals = true;
	bool renderFaceNormals = true;
	bool renderTextures = true;

	Texture* tex_info = nullptr;

};

class ComponentMesh : public Component {

public:
	
	ComponentMesh(GameObject* parent,const char* path);
	~ComponentMesh();

	bool Update(float dt);

public:
	const char* path_info;
	Mesh* mesh;
};