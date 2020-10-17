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

				mesh->GenerateBufferGeometry();
				geometry_storage.push_back(mesh);
			}
			
			// -- Copying Normals info --//
			if (new_mesh->HasNormals()) {
				
				//Initialize size
				mesh->normals = new float[new_mesh->mNumVertices * 3];
				//mesh->normal_faces = new float[mesh->num_index];
				//mesh->normal_face_vector_direction = new float[mesh->num_index];

				//Calculate Normals of Vertex
				for (size_t i = 0; i < new_mesh->mNumVertices; i++) {
					
					mesh->normals[i * 3] = new_mesh->mNormals[i].x;
					mesh->normals[i * 3 + 1] = new_mesh->mNormals[i].y;
					mesh->normals[i * 3 + 2] = new_mesh->mNormals[i].z;

				}
				//Calculate Normals of Face
				for (size_t i = 0; i < mesh->num_index; i++)
				{
					/*//Face point
					mesh->normal_faces[i] = (mesh->vertex[mesh->index[i * 3]] + (mesh->vertex[mesh->index[i * 3 + 1]] + (mesh->vertex[mesh->index[i * 3 + 2]]))) / 3;
					//Vector point
					float a = (mesh->vertex[mesh->index[i * 3 + 1]]) - (mesh->vertex[mesh->index[i * 3]]);
					float b = (mesh->vertex[mesh->index[i * 3 + 2]]) - (mesh->vertex[mesh->index[i * 3]]);*/
	
					
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

}

void Mesh::RenderGeometry() {

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->my_indices);

	glDrawElements(GL_TRIANGLES, this->num_index, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	/*if (renderVertexNormals) {

		glBegin(GL_LINES);
		glColor3f(1, 0, 1);

		for (size_t i = 0; i < this->num_vertex; i++)
		{
			glVertex3f(this->vertex[i * 3], this->vertex[i * 3 + 1], this->vertex[i * 3 + 2]);
			glVertex3f(this->vertex[i * 3] + this->normals[i * 3] * 0.15, this->vertex[i * 3 + 1] + this->normals[i * 3 + 1] * 0.15, this->vertex[i * 3 + 2] + this->normals[i * 3 + 2] * 0.15);
		}

		glColor3f(1, 1, 1);
		glEnd();
	}*/

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

