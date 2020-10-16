#include "Globals.h"
#include "Application.h"
#include "ModuleGeometry.h"
#include "ModuleWindow.h"

#include "SDL/include/SDL_opengl.h"
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

			//copy faces
			if (new_mesh->HasFaces()) {
				mesh->num_index = new_mesh->mNumFaces * 3;
				mesh->index = new uint[mesh->num_index];

				for (size_t i = 0; i < new_mesh->mNumFaces; i++)
				{
					if (new_mesh->mFaces[i].mNumIndices != 3) {
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else {
						memcpy(&mesh->index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));

					}
				}
				mesh->GenerateBufferGeometry(mesh);
			}
			
		}

		aiReleaseImport(scene);		

	}
	else {
		LOG("Error loading scene %s", path);
	}

	return UPDATE_CONTINUE;
}

void Mesh::GenerateBufferGeometry(Mesh* mesh) {

	mesh->my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(mesh->my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, mesh->my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 3, mesh->vertex, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	mesh->my_indices = 0;
	glGenBuffers(1, (GLuint*)&(mesh->my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_index, mesh->index, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->my_indices);

}

void Mesh::RenderGeometry(Mesh* mesh) {

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->my_indices);

	glDrawElements(GL_TRIANGLES, mesh->num_index, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

}

// Called before quitting
bool ModuleGeometry::CleanUp()
{
	//detach log stream
	aiDetachAllLogStreams();

	return true;
}

