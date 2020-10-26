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

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

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

	//Rendering Mesh Vector
	/*for (size_t i = 0; i < geometry_storage.size(); i++)
	{
		if (geometry_storage.at(i) != nullptr) 
			geometry_storage.at(i)->RenderGeometry();
	}*/

	//Rendering Primitive Vector
	for (size_t i = 0; i < primitives_storage.size(); i++)
	{
		if (primitives_storage.at(i) != nullptr) {

			switch (primitives_storage.at(i)->type) {
			case PrimitiveGL_Cube: {
					CubeGL * cube = static_cast<CubeGL*>(primitives_storage[i]);
					cube->InnerRender();
					break;
				}
			case PrimitiveGL_Sphere: {
				SphereGL* sphere = static_cast<SphereGL*>(primitives_storage[i]);
				sphere->InnerRender();
				break;
			}
			case PrimitiveGL_Pyramid: {
				PyramidGL* pyramid = static_cast<PyramidGL*>(primitives_storage[i]);
				pyramid->InnerRender();
				break;
			}
			case PrimitiveGL_Cylinder: {
				CylinderGL* cylinder = static_cast<CylinderGL*>(primitives_storage[i]);
				cylinder->InnerRender();
				break;
			}
			}
		}
	}

	return UPDATE_CONTINUE;
}

Mesh* ModuleGeometry::LoadGeometry(const char* path) {

	Mesh* mesh = new Mesh();

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	aiMesh* new_mesh = nullptr;

	if (scene != nullptr && scene->HasMeshes()) {
		
		//Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
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

			//Loading tex info into mesh
			if ((mesh->tex_info = App->textures->Load("Assets/Texture/Baker_house.png")) != nullptr) {
				LOG("Image texture data from mesh contains data");
			};

			//Last generate buffers
			App->scene->CreateGameObject(mesh, path);
		}

		aiReleaseImport(scene);		

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