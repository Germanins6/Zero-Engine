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
	mesh.id_index = 0;
	mesh.id_vertex = 0;
	mesh.index = nullptr;
	mesh.num_index = 0;
	mesh.num_vertex = 0;
	mesh.vertex = nullptr;
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

bool ModuleGeometry::LoadGeometry(const char* path) {

	const aiScene* scene = aiImportFile( path, aiProcessPreset_TargetRealtime_MaxQuality);
	aiMesh* new_mesh = nullptr;

	if (scene != nullptr && scene->HasMeshes()) {
		//Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			new_mesh = scene->mMeshes[i];
			mesh.num_vertex = new_mesh->mNumVertices;
			mesh.vertex = new float[mesh.num_vertex * 3];
			memcpy(mesh.vertex, new_mesh->mVertices, sizeof(float) * mesh.num_vertex * 3);
			LOG("New mesh with %d vertices", mesh.num_vertex);
		}
		aiReleaseImport(scene);		
	}

	//copy faces
	if (new_mesh->HasFaces()) {
		mesh.num_index = new_mesh->mNumFaces * 3;
		mesh.index = new uint[mesh.num_index];

		for (size_t i = 0; i < new_mesh->mNumFaces; i++)
		{
			if (new_mesh->mFaces[i].mNumIndices != 3) {
				LOG("WARNING, geometry face with != 3 indices!");
				return false;
			}
			else {
				memcpy(&mesh.index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
				return true;
			}
		}
	}
	//else {
	//	LOG("Error loading scene %s", "Assets/Models/warrior.FBX");
	//}

	

	DrawGeometry(mesh.vertex, mesh.index ,{1.f,1.f,1.f});
}

void ModuleGeometry::DrawGeometry(float vertex[], uint index[], vec3 pos) {

	uint my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.num_vertex, vertex, GL_STATIC_DRAW);

	uint my_indices = 0;
	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * mesh.num_index, index, GL_STATIC_DRAW);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glColor4f(App->editor->current_color.x, App->editor->current_color.y, App->editor->current_color.z, App->editor->current_color.w);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_QUADS, mesh.num_index, GL_UNSIGNED_SHORT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();

}
// PreUpdate: clear buffer
update_status ModuleGeometry::PreUpdate(float dt)
{
	

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleGeometry::PostUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleGeometry::CleanUp()
{
	//detach log stream
	aiDetachAllLogStreams();
	return true;
}

