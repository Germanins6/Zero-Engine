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

//Forward declaration
class ResourceMesh;

//Namespaces
using namespace math;

class ComponentMesh : public Component {

public:
	
	ComponentMesh(GameObject* parent, UID resourceMesh);
	~ComponentMesh();

	bool Update(float dt);

	// -- Generate/Render mesh
	void GenerateBufferGeometry();
	void RenderGeometry();
	void DrawVertexNormals();

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