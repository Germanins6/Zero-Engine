#include "Globals.h"

// -- Modules
#include "Application.h"
#include "ModuleGeometry.h"
#include "ModuleWindow.h"
#include "ModuleTextures.h"

// -- Tools
#include <vector>
#include "SDL/include/SDL_opengl.h"

//-- Assimp
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/mesh.h"

#pragma comment(lib, "Core/Assimp/libx86/assimp.lib")

ModuleGeometry::ModuleGeometry(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

// Destructor
ModuleGeometry::~ModuleGeometry()
{
	//-- Cleaning mesh vector
	for (size_t i = 0; i < geometry_storage.size(); i++) {
		geometry_storage[i] = nullptr;
	}
	geometry_storage.clear();


	//-- Cleaning primitives vector
	for (size_t i = 0; i < primitives_storage.size(); i++) {
		primitives_storage[i] = nullptr;
	}
	primitives_storage.clear();

}

// Called before render is available
bool ModuleGeometry::Init()
{
	//LOG("Creating 3D Renderer context");
	bool ret = true;

	//Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

update_status ModuleGeometry::Update(float dt) {

	return UPDATE_CONTINUE;
}

Mesh* ModuleGeometry::LoadGeometry(const char* path) {

	Mesh* mesh = nullptr;
	GameObject* root = nullptr;
	const aiScene* scene = nullptr;
	
	//Create path buffer and import to scene
	char* buffer = nullptr;
	uint bytesFile = App->file_system->Load(path, &buffer);

	if (buffer == nullptr) {
		string normalized_path(path);
		string norm_path_short = App->file_system->NormalizePath((normalized_path.substr(App->editor->ReturnNameObject(normalized_path, 0x5c) - 14)).c_str());

		bytesFile = App->file_system->Load(norm_path_short.c_str(), &buffer);
	}
	if (buffer != nullptr) {
		scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, NULL);
	}
	else {
		scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	}

	aiMesh* new_mesh = nullptr;

	if (scene != nullptr && scene->HasMeshes()) {
		
		if (scene->mNumMeshes > 1) {

			//If we have more than one mesh we create a root for the first element to parent later
			root = App->scene->CreateGameObject();			
		}

		//Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{

			mesh = new Mesh();
			mesh->num_meshes = scene->mNumMeshes;
			new_mesh = scene->mMeshes[i];
			mesh->num_vertex = new_mesh->mNumVertices;
			mesh->vertex = new float[mesh->num_vertex * 3];
			
			memcpy(mesh->vertex, new_mesh->mVertices, sizeof(float) * mesh->num_vertex * 3);
			LOG("New mesh with %d vertices", mesh->num_vertex);

			// -- Copying faces --//
			if (new_mesh->HasFaces()) {
				mesh->num_index = new_mesh->mNumFaces * 3;
				mesh->index = new uint[mesh->num_index];

				for (size_t i = 0; i < new_mesh->mNumFaces; i++)
				{
					if (new_mesh->mFaces[i].mNumIndices != 3) {
						LOG("WARNING, geometry face with != 3 indices!")
					}
					else {
						memcpy(&mesh->index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
				//LOG("%i", mesh->num_index);
				geometry_storage.push_back(mesh);
			}
			vec3 vert_suma;
			// -- Copying Normals info --//
			if (new_mesh->HasNormals()) {
				//Initialize size
				mesh->normals = new float[new_mesh->mNumVertices * 3];
				mesh->num_normal_faces = new_mesh->mNumFaces;

				//Calculate Normals of Vertex
				for (size_t i = 0; i < new_mesh->mNumVertices; i++) {
					
					//Calculate Normals of Vertex
					mesh->normals[i * 3] = new_mesh->mNormals[i].x;
					mesh->normals[i * 3 + 1] = new_mesh->mNormals[i].y;
					mesh->normals[i * 3 + 2] = new_mesh->mNormals[i].z;					
					
				}

				mesh->normal_faces = new float[new_mesh->mNumFaces * 3];
				mesh->normal_face_vector_direction = new float[new_mesh->mNumFaces * 3];

				for (size_t i = 0; i < new_mesh->mNumFaces; i++)
				{
					//Calculate Normals of Face
					//Face point
					vec3 vert1_center = { mesh->vertex[i * 3],mesh->vertex[i * 3 + 1], mesh->vertex[i * 3 + 2] };
					vec3 vert2_center = { mesh->vertex[i * 3 + 3],mesh->vertex[i * 3 + 4], mesh->vertex[i * 3 + 5] };
					vec3 vert3_center = { mesh->vertex[i * 3 + 6], mesh->vertex[i * 3 + 7], mesh->vertex[i * 3 + 8] };
					vec3 result_center = (vert1_center + vert2_center + vert3_center) / 3;

					mesh->normal_faces[i * 3] = result_center.x;
					mesh->normal_faces[i * 3 + 1] = result_center.y;
					mesh->normal_faces[i * 3 + 2] = result_center.z;

					//Vector point
					vec3 vert1_normal = { mesh->normals[i * 3] , mesh->normals[i * 3 + 1] , mesh->normals[i * 3 + 2] };
					vec3 vert2_normal = { mesh->normals[i * 3 + 3] , mesh->normals[i * 3 + 4] ,mesh->normals[i * 3 + 5]};
					vec3 vert3_normal = { mesh->normals[i * 3 + 6] , mesh->normals[i * 3 + 7] ,mesh->normals[i * 3 + 8] };
					vec3 result_normal = (vert1_normal + vert2_normal + vert3_normal) / 3;

					////Vector point
					//vec3 a = vert2_center - vert1_center;
					//vec3 b = vert3_center - vert1_center;
					//vec3 normal = cross(a, b);
					//vec3 result_normal = normalize(normal);

					mesh->normal_face_vector_direction[i * 3] = result_normal.x;
					mesh->normal_face_vector_direction[i * 3 + 1] = result_normal.y;
					mesh->normal_face_vector_direction[i * 3 + 2] = result_normal.z;

				}
				
			}
			
			mesh->uv_coords = new float[mesh->num_vertex * 2];
			for (size_t i = 0; i < new_mesh->mNumVertices; i++) {

				if (new_mesh->mTextureCoords[0]) {
					mesh->uv_coords[i * 2] = new_mesh->mTextureCoords[0][i].x;
					mesh->uv_coords[i * 2 + 1] = new_mesh->mTextureCoords[0][i].y;
				}
			}

			//If we have a root we parent each mesh in each cycle with this gameObject, if not we create single unparent gameObject
			if (root != nullptr)
				App->scene->CreateGameObject(mesh, path, root);
			else 
				App->scene->CreateGameObject(mesh, path);
			
		}

		aiReleaseImport(scene);		
		RELEASE_ARRAY(buffer);
	}
	else {
		LOG("Error loading scene %s", path);
	}

	return mesh;
}

// Called before quitting
bool ModuleGeometry::CleanUp()
{

	//Remember clean mesh when drag and drop!!
	//REMEMBER CLEAN ALL BUFFERS AND THINGS TODO

	//detach log stream
	aiDetachAllLogStreams();

	return true;
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
	
	for (size_t i = 0; i < 24; i++)
	{
		mesh->vertex = vertex;
	}
	for (size_t i = 0; i < 36; i++)
	{
		mesh->index = index;
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

	for (size_t i = 0; i < 15; i++)
	{
		mesh->vertex = points;
	}
	for (size_t i = 0; i < 18; i++)
	{
		mesh->index = indices;
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



