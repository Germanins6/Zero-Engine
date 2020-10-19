#include "Globals.h"
#include "Application.h"
#include "ModuleGeometry.h"
#include "ModuleWindow.h"
#include <vector>
#include "SDL/include/SDL_opengl.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/mesh.h"

#pragma comment(lib, "Core/Assimp/libx86/assimp.lib")

ModuleGeometry::ModuleGeometry(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	geometry_data = new Mesh();

}

// Destructor
ModuleGeometry::~ModuleGeometry()
{}

// Called before render is available
bool ModuleGeometry::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

update_status ModuleGeometry::Update(float dt) {

	//Just renders geometry when filled with something
	for (size_t i = 0; i < geometry_storage.size(); i++)
	{
		if (geometry_storage.at(i) != nullptr) {

			geometry_storage.at(i)->RenderGeometry();

		}

	}

	return UPDATE_CONTINUE;
}

bool ModuleGeometry::LoadGeometry(Mesh* mesh, const char* path) {

	const aiScene* scene = aiImportFile( path, aiProcessPreset_TargetRealtime_MaxQuality);
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
					if (new_mesh->mFaces[i].mNumIndices != 3) 
						LOG("WARNING, geometry face with != 3 indices!")
					else 
						memcpy(&mesh->index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
				}
				LOG("%i", mesh->num_index);
				mesh->GenerateBufferGeometry();
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
					float vx = (mesh->vertex[i * 3] + mesh->vertex[i * 3 + 3] + mesh->vertex[i * 3 + 6]) / 3;
					float vy = (mesh->vertex[i * 3 + 1] + mesh->vertex[i * 3 + 4] + mesh->vertex[i * 3 + 7]) / 3;
					float vz = (mesh->vertex[i * 3 + 2] + mesh->vertex[i * 3 + 5] + mesh->vertex[i * 3 + 8]) / 3;

					LOG("%f %f %f", vx, vy, vz);
					 
					mesh->normal_faces[i * 3] = vx;
					mesh->normal_faces[i * 3 + 1] = vy;
					mesh->normal_faces[i * 3 + 2] = vz;
					
					//Vector point
					vec3 vert1_normal = { mesh->normals[i * 3] , mesh->normals[i * 3 + 1] , mesh->normals[i * 3 + 2] };
					vec3 vert2_normal = { mesh->normals[i * 3 + 3] , mesh->normals[i * 3 + 4] ,mesh->normals[i * 3 + 5]};
					vec3 vert3_normal = { mesh->normals[i * 3 + 6] , mesh->normals[i * 3 + 7] ,mesh->normals[i * 3 + 8] };
					vec3 result = (vert1_normal + vert2_normal + vert3_normal) / 3;

					mesh->normal_face_vector_direction[i * 3] = result.x;
					mesh->normal_face_vector_direction[i * 3 + 1] = result.y;
					mesh->normal_face_vector_direction[i * 3 + 2] = result.z;

					LOG("NORMAL FACE: %f %f %f", result.x, result.y, result.z);

				}
				
			}
			
		}

		aiReleaseImport(scene);		

	}
	else {
		LOG("Error loading scene %s", path);
	}

	return UPDATE_CONTINUE;
}

void Mesh::GenerateBufferGeometry() {

	//-- Generate Vertex
	this->my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(this->my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, this->my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->num_vertex * 3, this->vertex, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//-- Generate Index
	this->my_indices = 0;
	glGenBuffers(1, (GLuint*)&(this->my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * this->num_index, this->index, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->my_indices);

	//-- Generate Texture
	glGenTextures(1, (GLuint*)&(this->textureID));
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

}

void Mesh::RenderGeometry() {

	// -- Geometry Rendering -- //
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->my_indices);

	glDrawElements(GL_TRIANGLES, this->num_index, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	// -- Vertex Normals Rendering -- //
	if (renderVertexNormals) {

		glBegin(GL_LINES);
		glColor3f(1, 0, 1);

		for (size_t i = 0; i < this->num_vertex; i++)
		{
			glVertex3f(this->vertex[i * 3], this->vertex[i * 3 + 1], this->vertex[i * 3 + 2]);
			glVertex3f(this->vertex[i * 3] + this->normals[i * 3] * 0.15, this->vertex[i * 3 + 1] + this->normals[i * 3 + 1] * 0.15, this->vertex[i * 3 + 2] + this->normals[i * 3 + 2] * 0.15);
		}

		glColor3f(1, 1, 1);
		glEnd();
	}

	// -- Face Normals Rendering -- //
	if (renderFaceNormals) {

		glBegin(GL_LINES);
		glColor3f(1, 0, 1);

		for (size_t i = 0; i < this->num_normal_faces; i++)
		{
			glVertex3f(this->normal_faces[i * 3], this->normal_faces[i * 3 + 1], this->normal_faces[i * 3 + 2]);
			glVertex3f(
				this->normal_faces[i * 3] + this->normal_face_vector_direction[i * 3] * 0.15f, 
				this->normal_faces[i * 3 + 1] + this->normal_face_vector_direction[i * 3 + 1] * 0.15f, 
				this->normal_faces[i * 3 + 2] + this->normal_face_vector_direction[i * 3 + 2] * 0.15f
			);
		}

		glColor3f(1, 1, 1);
		glEnd();
	}

	// -- Texture Rendering -- //
	if (renderTextures) {
		//glEnable(GL_TEXTURE_2D);
	}

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

