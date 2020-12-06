#pragma once
#include "Component.h"

// -- Tools
#include "Globals.h"

#include "PrimitivesGL.h"
#include "glmath.h"
#include <string.h>

//MathgeoLib
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/OBB.h"

using namespace math;

class Mesh {

public:

	Mesh();
	~Mesh();

	

	void GenerateTextureInfo();
	void GenerateCheckers();

	

public:

	
	
	

	uint textureID;

	//bool renderTextures;

	
	//bool draw_texture;
	//bool draw_checkers;
	//GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];




};

class ComponentMesh : public Component {

public:
	
	ComponentMesh(GameObject* parent, ResourceMesh* resourceMesh);
	~ComponentMesh();

	bool Update(float dt);

	// -- Generate/Render mesh
	void GenerateBufferGeometry();
	void RenderGeometry();

	// -- Bounding Box generation
	void GenerateAABB();
	inline AABB GetAABB() { return bbox; };

public:

	ResourceMesh* ourMesh;

	//Mesh settings for inspector
	bool draw_mesh;
	bool draw_vertexNormals;

	//Buffers
	uint my_vertex;
	uint my_indices;
	uint my_normals;
	uint my_texture;

	//Bounding box
	AABB bbox;

};