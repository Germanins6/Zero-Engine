#pragma once
#include "Component.h"

#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128
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

class Mesh {

public:

	Mesh();
	~Mesh();

	void GenerateBufferGeometry();
	void GenerateBufferPrimitives();
	void RenderGeometry();
	void RenderPrimitives();

	void GenerateTextureInfo();
	void GenerateCheckers();

public:
	uint id_index; //index in VRAM
	uint num_index;
	uint* index;

	uint id_vertex; 
	uint num_vertex;
	float* vertex;

	uint id_normals;
	uint num_normals;
	uint num_normal_faces;

	float* normals;
	float* normal_face_vector_direction;
	float* normal_faces;

	uint my_vertex;
	uint my_indices;
	uint my_normals;
	uint my_texture;

	uint textureID;
	float* uv_coords;

	bool renderTextures;

	Texture* tex_info;
	
	bool draw_texture;
	bool draw_checkers;
	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	PrimitiveTypesGL type;

	int num_meshes;

	string texture_path;
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