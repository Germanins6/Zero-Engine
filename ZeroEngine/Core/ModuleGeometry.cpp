#include "Globals.h"

// -- Modules
#include "Application.h"
#include "ModuleGeometry.h"
#include "ModuleWindow.h"
#include "Textures.h"

// -- Tools
#include <vector>
#include "SDL/include/SDL_opengl.h"

ModuleGeometry::ModuleGeometry(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

// Destructor
ModuleGeometry::~ModuleGeometry()
{
	
	//-- Cleaning primitives vector
	for (size_t i = 0; i < primitives_storage.size(); i++)
		RELEASE(primitives_storage[i]);

	primitives_storage.clear();

}

Mesh* ModuleGeometry::CubeGL(){

	Mesh* mesh = new Mesh();

	float vertex[24]{

		0.f, 0.f, 0.f,  //A
		1.f, 0.f, 0.f,  //B
		1.f, 1.f, 0.f,  //C
		0.f, 1.f, 0.f,  //D

		0.f, 0.f, -1.f,  //E
		1.f, 0.f, -1.f,  //F
		0.f, 1.f, -1.f,  //G
		1.f, 1.f, -1.f,  //H

	};

	uint index[36]{

		//FRONT
		0, 1, 2,
		0, 2, 3,

		//RIGHT
		4, 0, 3,
		4, 3, 6,

		//LEFT
		1, 5, 7,
		1, 7, 2,

		//BACK
		5, 4, 6,
		5, 6, 7,

		//DOWN
		0, 4, 5,
		0, 5, 1,

		//UP
		3, 2, 7,
		3, 7, 6,

	};
	
	mesh->num_vertex = 24;
	mesh->num_index = 36;
	
	float* vertices_ = new float[mesh->num_vertex];
	uint* index_ = new uint[mesh->num_index];

	for (size_t i = 0; i < mesh->num_vertex; i++)
	{
		vertices_[i] = vertex[i];
		mesh->vertex = vertices_;
	}
	for (size_t i = 0; i < mesh->num_index; i++)
	{
		index_[i] = index[i];
		mesh->index = index_;
	}

	primitives_storage.push_back(mesh);
	//Last generate buffers
	App->scene->CreateGameObject(PrimitiveTypesGL::PrimitiveGL_Cube, mesh);
	
	return mesh;
}
Mesh* ModuleGeometry::SphereGL() {
	
	float radius = 1.f;
	int sectors = 20;
	int rings = 20;

	Mesh* mesh = new Mesh();
	vector<float> vertices, normals, texCoords;
	vector<short> indices;

	// clear memory of prev arrays
	std::vector<float>().swap(vertices);
	std::vector<float>().swap(normals);
	std::vector<float>().swap(texCoords);

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * PI / sectors;
	float stackStep = PI / rings;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= rings; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			normals.push_back(nx);
			normals.push_back(ny);
			normals.push_back(nz);

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectors;
			t = (float)i / rings;
			texCoords.push_back(s);
			texCoords.push_back(t);
		}
	}

	// generate CCW index list of sphere triangles

	int k1, k2;
	for (int i = 0; i < rings; ++i)
	{
		k1 = i * (sectors + 1);     // beginning of current stack
		k2 = k1 + sectors + 1;      // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (rings - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	mesh->num_vertex = vertices.size();
	mesh->num_index = indices.size();
	float* vertex_ = new float[mesh->num_vertex];
	uint* index_ = new uint[mesh->num_index];

	for (size_t i = 0; i < mesh->num_vertex; i++)
	{
		vertex_[i] = vertices[i];
		mesh->vertex = vertex_;
	}

	for (size_t i = 0; i < mesh->num_index; i++)
	{
		index_[i] = indices[i];
		mesh->index = index_;
	}

	primitives_storage.push_back(mesh);
	//Last generate buffers
	App->scene->CreateGameObject(PrimitiveTypesGL::PrimitiveGL_Sphere, mesh);

	return mesh;
}
Mesh* ModuleGeometry::PyramidGL() {
	
	Mesh* mesh = new Mesh();
	float points[15]{

		  0.f, 0.f, 0.f, //A
		  1.f, 0.f, 0.f, //B

		  0.f, 0.f, -1.f, //C
		  1.f, 0.f, -1.f, //D

		  0.5f,1.f, -0.5f, //E

	};

	uint indices[18]{

		0,1,4, //Front
		2,0,4, //Right
		1,3,4, //Left
		3,2,4, //Back

		//Down
		0,3,1,
		0,2,3,

	};

	mesh->num_vertex = 15;
	mesh->num_index = 18;

	float* vertices_ = new float[mesh->num_vertex];
	uint* index_ = new uint[mesh->num_index];

	for (size_t i = 0; i < mesh->num_vertex; i++)
	{
		vertices_[i] = points[i];
		mesh->vertex = vertices_;
	}
	for (size_t i = 0; i < mesh->num_index; i++)
	{
		index_[i] = indices[i];
		mesh->index = index_;
	}
	primitives_storage.push_back(mesh);
	//Last generate buffers
	App->scene->CreateGameObject(PrimitiveTypesGL::PrimitiveGL_Pyramid, mesh);

	return mesh;
}
Mesh* ModuleGeometry::CylinderGL() {

	Mesh* mesh = new Mesh();

	int sectorCount = 30;
	float radius = 1.f;
	float height = 1.f;

	std::vector<float> vertices, normals, texCoords;
	std::vector<short>	indices;

	float sectorStep = 2 * PI / sectorCount;
	float sectorAngle;  // radian

	std::vector<float> unitCircleVertices;
	for (int i = 0; i <= sectorCount; ++i)
	{
		sectorAngle = i * sectorStep;
		unitCircleVertices.push_back(cos(sectorAngle)); // x
		unitCircleVertices.push_back(sin(sectorAngle)); // y
		unitCircleVertices.push_back(0);                // z
	}

	// clear memory of prev arrays
	std::vector<float>().swap(vertices);

	// get unit circle vectors on XY-plane
	std::vector<float> unitVertices = unitCircleVertices;

	// put side vertices to arrays
	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
		float t = 1.0f - i;                              // vertical tex coord; 1 to 0

		for (int j = 0, k = 0; j <= sectorCount; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uy = unitVertices[k + 1];
			float uz = unitVertices[k + 2];
			// position vector
			vertices.push_back(ux * radius);             // vx
			vertices.push_back(uy * radius);             // vy
			vertices.push_back(h);                       // vz
			// normal vector
			normals.push_back(ux);                       // nx
			normals.push_back(uy);                       // ny
			normals.push_back(uz);                       // nz
			// texture coordinate
			texCoords.push_back((float)j / sectorCount); // s
			texCoords.push_back(t);                      // t

		}
	}

	// the starting index for the base/top surface
	//NOTE: it is used for generating indices later
	int baseCenterIndex = (int)vertices.size() / 3;
	int topCenterIndex = baseCenterIndex + sectorCount + 1; // include center vertex

	// put base and top vertices to arrays
	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
		float nz = -1 + i * 2;                           // z value of normal; -1 to 1

		// center point
		vertices.push_back(0);     vertices.push_back(0);     vertices.push_back(h);
		normals.push_back(0);      normals.push_back(0);      normals.push_back(nz);
		texCoords.push_back(0.5f); texCoords.push_back(0.5f);

		for (int j = 0, k = 0; j < sectorCount; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uy = unitVertices[k + 1];
			// position vector
			vertices.push_back(ux * radius);             // vx
			vertices.push_back(uy * radius);             // vy
			vertices.push_back(h);                       // vz
			// normal vector
			normals.push_back(0);                        // nx
			normals.push_back(0);                        // ny
			normals.push_back(nz);                       // nz
			// texture coordinate
			texCoords.push_back(-ux * 0.5f + 0.5f);      // s
			texCoords.push_back(-uy * 0.5f + 0.5f);      // t
		}
	}

	int k1 = 0;                         // 1st vertex index at base
	int k2 = sectorCount + 1;           // 1st vertex index at top

	// indices for the side surface
	for (int i = 0; i < sectorCount; ++i, ++k1, ++k2)
	{
		// 2 triangles per sector
		// k1 => k1+1 => k2
		indices.push_back(k1);
		indices.push_back(k1 + 1);
		indices.push_back(k2);

		// k2 => k1+1 => k2+1
		indices.push_back(k2);
		indices.push_back(k1 + 1);
		indices.push_back(k2 + 1);
	}

	// indices for the base surface
	//NOTE: baseCenterIndex and topCenterIndices are pre-computed during vertex generation
	//      please see the previous code snippet
	for (int i = 0, k = baseCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
		{
			indices.push_back(baseCenterIndex);
			indices.push_back(k + 1);
			indices.push_back(k);
		}
		else // last triangle
		{
			indices.push_back(baseCenterIndex);
			indices.push_back(baseCenterIndex + 1);
			indices.push_back(k);
		}
	}

	// indices for the top surface
	for (int i = 0, k = topCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
		{
			indices.push_back(topCenterIndex);
			indices.push_back(k);
			indices.push_back(k + 1);
		}
		else // last triangle
		{
			indices.push_back(topCenterIndex);
			indices.push_back(k);
			indices.push_back(topCenterIndex + 1);
		}
	}

	mesh->num_vertex = vertices.size();
	mesh->num_index = indices.size();
	float* vertex_ = new float[mesh->num_vertex];
	uint* index_ = new uint[mesh->num_index];

	for (size_t i = 0; i < mesh->num_vertex; i++)
	{
		vertex_[i] = vertices[i];
		mesh->vertex = vertex_;
	}

	for (size_t i = 0; i < mesh->num_index; i++)
	{
		index_[i] = indices[i];
		mesh->index = index_;
	}

	primitives_storage.push_back(mesh);
	//Last generate buffers
	App->scene->CreateGameObject(PrimitiveTypesGL::PrimitiveGL_Cylinder, mesh);

	return mesh;
}