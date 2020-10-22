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
	geometry_data = new Mesh();

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
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

update_status ModuleGeometry::Update(float dt) {

	//Rendering Mesh Vector
	for (size_t i = 0; i < geometry_storage.size(); i++)
	{
		if (geometry_storage.at(i) != nullptr) 
			geometry_storage.at(i)->RenderGeometry();
	}

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

bool ModuleGeometry::LoadGeometry(Mesh* mesh, const char* path) {


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
					if (new_mesh->mFaces[i].mNumIndices != 3) 
						LOG("WARNING, geometry face with != 3 indices!")
					else 
						memcpy(&mesh->index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
				}
				LOG("%i", mesh->num_index);
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
					/*//Calculate Normals of Face
					//Face point
					vec3 vert1_center = { mesh->vertex[i * 3],mesh->vertex[i * 3 + 1], mesh->vertex[i * 3 + 2] };
					vec3 vert2_center = { mesh->vertex[i * 3 + 3],mesh->vertex[i * 3 + 4], mesh->vertex[i * 3 + 5] };
					vec3 vert3_center = { mesh->vertex[i * 3 + 6], mesh->vertex[i * 3 + 7], mesh->vertex[i * 3 + 8] };
					vec3 result_center = (vert1_center + vert2_center + vert3_center) / 3;
					//LOG("%f %f %f", vert1_center.x, vert1_center.y, vert1_center.z);

					mesh->normal_faces[i * 3] = result_center.x;
					mesh->normal_faces[i * 3 + 1] = result_center.y;
					mesh->normal_faces[i * 3 + 2] = result_center.z;

					//Vector point
					vec3 vert1_normal = { mesh->normals[i * 3] , mesh->normals[i * 3 + 1] , mesh->normals[i * 3 + 2] };
					vec3 vert2_normal = { mesh->normals[i * 3 + 3] , mesh->normals[i * 3 + 4] ,mesh->normals[i * 3 + 5]};
					vec3 vert3_normal = { mesh->normals[i * 3 + 6] , mesh->normals[i * 3 + 7] ,mesh->normals[i * 3 + 8] };
					vec3 result_normal = (vert1_normal + vert2_normal + vert3_normal) / 3;
					//LOG("NORMAL FACE: %f %f %f", result_normal.x, result_normal.y, result_normal.z);
					//Vector point
					/*vec3 a = vert2_center - vert1_center;
					vec3 b = vert3_center - vert1_center;
					vec3 normal = cross(a, b);
					vec3 result_normal = normalize(normal);
					//LOG("%f %f %f", normalized_normal.x, normalized_normal.y, normalized_normal.z);
					
					mesh->normal_face_vector_direction[i * 3] = result_normal.x;
					mesh->normal_face_vector_direction[i * 3 + 1] = result_normal.y;
					mesh->normal_face_vector_direction[i * 3 + 2] = result_normal.z;*/

				}
				
			}
			
			mesh->uv_coords = new float[mesh->num_vertex * 2];
			for (size_t i = 0; i < new_mesh->mNumVertices; i++) {

				if (new_mesh->mTextureCoords[0]) {
					mesh->uv_coords[i * 2] = new_mesh->mTextureCoords[0][i].x;
					mesh->uv_coords[i * 2 + 1] = new_mesh->mTextureCoords[0][i].y;
				}

			}
			//Loading tex info
			mesh->tex_info = App->textures->Load("Assets/Texture/Baker_house.png");

			//Last generate buffers
			mesh->GenerateBufferGeometry();
		}

		aiReleaseImport(scene);		

	}
	else {
		LOG("Error loading scene %s", path);
	}

	return UPDATE_CONTINUE;
}

void Mesh::GenerateBufferGeometry() {

	//-- Generate Normals
	this->my_normals = 0;
	glGenBuffers(1, (GLuint*)&(this->my_normals));
	glBindBuffer(GL_NORMAL_ARRAY, this->my_normals);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * this->num_normals * 3, this->normals, GL_STATIC_DRAW);

	//-- Generate Vertex
	this->my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(this->my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, this->my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->num_vertex * 3, this->vertex, GL_STATIC_DRAW);
	
	

	//-- Generate Index
	this->my_indices = 0;
	glGenBuffers(1, (GLuint*)&(this->my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * this->num_index, this->index, GL_STATIC_DRAW);
	

	//-- Generate Texture_Buffers
	this->my_texture = 0;
	glGenBuffers(1, (GLuint*)&(this->my_texture));
	glBindBuffer(GL_ARRAY_BUFFER, this->my_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * this->num_vertex, this->uv_coords, GL_STATIC_DRAW);

	//-- Generate Texture
	//this->tex_info->id = 0;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, (GLuint*)&(this->tex_info->id));
	glBindTexture(GL_TEXTURE_2D, (uint)&this->tex_info->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)this->tex_info->GetWidth(), (int)this->tex_info->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, this->tex_info->data);

}

void Mesh::RenderGeometry() {

	// -- Geometry Rendering -- //
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, this->my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_NORMAL_ARRAY, this->my_normals);
	glNormalPointer(GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, this->my_texture);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->my_indices);
	
	glDrawElements(GL_TRIANGLES, this->num_index, GL_UNSIGNED_INT, NULL);
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	// -- Vertex Normals Rendering -- //
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

	// -- Face Normals Rendering -- //
	/*if (renderFaceNormals) {

		glBegin(GL_LINES);
		glColor3f(1, 0, 1);

		for (size_t i = 0; i < this->num_normal_faces; i++)
		{
			glVertex3f(this->normal_faces[i * 3], this->normal_faces[i * 3 + 1], this->normal_faces[i * 3 + 2]);
			glVertex3f(
				this->normal_faces[i * 3] + this->normal_face_vector_direction[i * 3] * 0.15, 
				this->normal_faces[i * 3 + 1] + this->normal_face_vector_direction[i * 3 + 1] * 0.15, 
				this->normal_faces[i * 3 + 2] + this->normal_face_vector_direction[i * 3 + 2] * 0.15
			);
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