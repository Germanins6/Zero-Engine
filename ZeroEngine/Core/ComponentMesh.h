#pragma once
#include "Component.h"

#define CHECKERS_HEIGHT 32
#define CHECKERS_WIDTH 32
// -- Tools
#include "Globals.h"

#include "PrimitivesGL.h"
#include "Light.h"
#include "glmath.h"
#include <string.h>

// -- ModuleTextures
#include "ModuleTextures.h"

enum PrimitiveTypesGL
{
	PrimitiveGL_NONE,
	PrimitiveGL_Cube,
	PrimitiveGL_Sphere,
	PrimitiveGL_Triangle_pyramid,
	PrimitiveGL_Pyramid,
	PrimitiveGL_Cylinder
};

struct Mesh {

	void GenerateBufferGeometry();
	void GenerateBufferPrimitives();
	void RenderGeometry();
	void RenderPrimitives();

	void GenerateTextureInfo();
	void GenerateCheckers();

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

	bool renderTextures = true;

	Texture* tex_info = nullptr;
	
	bool draw_texture = true;
	bool draw_checkers = false;
	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	PrimitiveTypesGL type = PrimitiveTypesGL::PrimitiveGL_NONE;
};

class ComponentMesh : public Component {

public:
	
	ComponentMesh(GameObject* parent, Mesh* data,const char* path);
	ComponentMesh(GameObject* parent, Mesh* data);
	~ComponentMesh();

	bool Update(float dt);

public:
	const char* path_info;
	Mesh* mesh;

	bool draw_vertexNormals;
	bool draw_faceNormals;
	bool draw_mesh;
	
};