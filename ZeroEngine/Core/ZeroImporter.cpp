#include "Application.h"
#include "ZeroImporter.h"
#include "ComponentMesh.h"

//-- Assimp
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/mesh.h"

#pragma comment(lib, "Core/Assimp/libx86/assimp.lib")

// -- DevIL Image Library
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

//-- Devil Libs loading
#pragma comment(lib, "Core/DevIL/libx86/DevIL.lib")
#pragma comment(lib, "Core/DevIL/libx86/ILU.lib")
#pragma comment(lib, "Core/DevIL/libx86/ILUT.lib")

// ==== MESH ==== //

void MeshImporter::Init() {

	//Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

}

void MeshImporter::CleanUp() {

	//-- Detach log stream
	aiDetachAllLogStreams();
}

void MeshImporter::Import(const aiMesh* aiMesh, ResourceMesh* ourMesh){

	//Checking how long takes to import normal fbx
	Timer importTime;
	importTime.Start();

	//Vertex
	ourMesh->num_vertex = aiMesh->mNumVertices;
	ourMesh->vertex = new float[ourMesh->num_vertex * 3];
	memcpy(ourMesh->vertex, aiMesh->mVertices, sizeof(float) * ourMesh->num_vertex * 3);
	LOG("New mesh with %d vertices", ourMesh->num_vertex);

	//Index
	if (aiMesh->HasFaces()) {
		ourMesh->num_index = aiMesh->mNumFaces * 3;
		ourMesh->index = new uint[ourMesh->num_index];

		for (size_t i = 0; i < aiMesh->mNumFaces; i++)
		{
			if (aiMesh->mFaces[i].mNumIndices != 3)
				LOG("WARNING, geometry face with != 3 indices!")
			else
				memcpy(&ourMesh->index[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
		}
	}

	//Normals
	if (aiMesh->HasNormals()) {

		//Initialize size
		ourMesh->num_normals = aiMesh->mNumVertices;
		ourMesh->normals = new float[aiMesh->mNumVertices * 3];

		//Calculate Normals of Vertex
		for (size_t i = 0; i < aiMesh->mNumVertices; i++) {

			//Calculate Normals of Vertex
			ourMesh->normals[i * 3] = aiMesh->mNormals[i].x;
			ourMesh->normals[i * 3 + 1] = aiMesh->mNormals[i].y;
			ourMesh->normals[i * 3 + 2] = aiMesh->mNormals[i].z;
		}
	}

	//Uvs
	ourMesh->num_uvs = aiMesh->mNumVertices;
	ourMesh->uv_coords = new float[ourMesh->num_vertex * 2];

	for (size_t i = 0; i < aiMesh->mNumVertices; i++) {

		if (aiMesh->mTextureCoords[0]) {
			ourMesh->uv_coords[i * 2] = aiMesh->mTextureCoords[0][i].x;
			ourMesh->uv_coords[i * 2 + 1] = aiMesh->mTextureCoords[0][i].y;
		}
	}

	LOG("FBX took %d ms to be imported", importTime.Read());
}

uint64 MeshImporter::Save(const ResourceMesh* ourMesh, char** fileBuffer) {

	uint ranges[4] = { ourMesh->num_index, ourMesh->num_vertex, ourMesh->num_normals, ourMesh->num_uvs};

	uint size = sizeof(ranges) + sizeof(uint) * ourMesh->num_index + sizeof(float) * ourMesh->num_vertex * 3 + sizeof(float) * ourMesh->num_normals * 3 + sizeof(float) * ourMesh->num_uvs * 2;

	char* BufferSize = new char[size];
	char* cursor = BufferSize;

	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	//Index
	bytes = sizeof(uint) * ourMesh->num_index;
	memcpy(cursor, ourMesh->index, bytes);
	cursor += bytes;

	//Vertex
	bytes = sizeof(float) * ourMesh->num_vertex * 3;
	memcpy(cursor, ourMesh->vertex, bytes);
	cursor += bytes;

	//Normals
	bytes = sizeof(float) * ourMesh->num_normals * 3;
	memcpy(cursor, ourMesh->normals, bytes);
	cursor += bytes;

	//Uvs
	bytes = sizeof(float) * ourMesh->num_uvs * 2;
	memcpy(cursor, ourMesh->uv_coords, bytes);
	cursor += bytes;

	*fileBuffer = BufferSize;

	return size;
}

void MeshImporter::Load(const char* fileBuffer, Mesh* ourMesh) {

	//Checking how long takes to load our own mesh structure
	Timer loadTime;
	loadTime.Start();

	//Creating buffer to store fileBuffer info into to be used later by our cursor to load mesh info
	char* buffer;
	App->file_system->Load(fileBuffer, &buffer);
	char* cursor = buffer;

	//Mirror steps as Save
	uint ranges[4];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	ourMesh->num_index = ranges[0];
	ourMesh->num_vertex = ranges[1];
	ourMesh->num_normals = ranges[2];
	ourMesh->num_uvs = ranges[3];

	//Index
	bytes = sizeof(uint) * ourMesh->num_index;
	ourMesh->index = new uint[ourMesh->num_index];
	memcpy(ourMesh->index, cursor, bytes);
	cursor += bytes;

	//Vertex
	bytes = sizeof(float) * ourMesh->num_vertex * 3;
	ourMesh->vertex = new float[ourMesh->num_vertex * 3];
	memcpy(ourMesh->vertex, cursor, bytes);
	cursor += bytes;

	//Normals
	bytes = sizeof(float) * ourMesh->num_normals * 3;
	ourMesh->normals = new float[ourMesh->num_normals * 3];
	memcpy(ourMesh->normals, cursor, bytes);
	cursor += bytes;

	//Uvs
	bytes = sizeof(float) * ourMesh->num_uvs * 2;
	ourMesh->uv_coords = new float[ourMesh->num_uvs * 2];
	memcpy(ourMesh->uv_coords, cursor, bytes);
	cursor += bytes;

	//meshes.push_back(ourMesh);

	LOG("Own file took %d ms to be loaded", loadTime.Read());
}


// ==== TEXTURE ==== //


void TextureImporter::Init() {

	//Devil Ilu and Il initialization
	ilInit();
	iluInit();
	ilutInit();

	if (ilutRenderer(ILUT_OPENGL))
		LOG("DevIL succesfully loaded: Renderer set as OpenGL | Libs initialized")
	else 
		LOG("Error trying load DevIL renderer");

}

void TextureImporter::CleanUp() {

	//Cleaning texture buffers and vector
	/*for (size_t i = 0; i < textures.size(); i++)
		RELEASE(textures[i])

		textures.clear();*/
}

void TextureImporter::Import(const char* path) {

	Timer imageImport;
	imageImport.Start();

	char* buffer = nullptr;
	uint size = App->file_system->Load(path, &buffer);

	if (buffer != nullptr) {
		if (ilLoadL(IL_TYPE_UNKNOWN, buffer, size) == IL_FALSE) {
			if (ilLoadImage(path) == IL_FALSE)
				LOG("Source image from %s path Imported Succesfully", path)
			else
				LOG("Unable to import texture");
		}
	}

	LOG("Image file took %d ms to be imported", imageImport.Read());
}

uint64 TextureImporter::Save(char** fileBuffer) {

	ILuint size;

	ILubyte* data;


	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, nullptr, 0);

	if (size > 0) {

		data = new ILubyte[size];
		if (ilSaveL(IL_DDS, data, size) > 0)
			*fileBuffer = (char*)data;
	}

	return size;
}

void TextureImporter::Load(const char* fileBuffer, Texture* ourTexture) {

	Timer imageLoad;
	imageLoad.Start();

	ILuint temp = 0;
	ilGenImages(1, &temp);
	ilBindImage(temp);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	char* buffer;
	uint size = App->file_system->Load(fileBuffer, &buffer);
	
	if (ilLoadL(IL_DDS, buffer, size) == IL_FALSE)
		LOG("Unable to load texture");

	//Initialitizing texture values and buff
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	ourTexture->id = temp;
	ourTexture->height = ilGetInteger(IL_IMAGE_HEIGHT);
	ourTexture->width = ilGetInteger(IL_IMAGE_WIDTH);
	ourTexture->type = ilGetInteger(IL_IMAGE_FORMAT);
	ourTexture->data = ilGetData();

	//textures.push_back(ourTexture);

	ilBindImage(0);

	LOG("Succesfully image loaded with: ID %u SIZE %u X %u", ourTexture->id, ourTexture->width, ourTexture->height);
	LOG("Image file took %d ms to be loaded", imageLoad.Read());
}

// ==== MODEL ==== //

void ModelImporter::Import(const char* path, ResourceModel* ourModel) {

	Timer modelImport;
	modelImport.Start();

	const aiScene* scene = nullptr;

	char* buffer;
	string prueba = App->file_system->NormalizePath(path);
	prueba = App->file_system->SetNormalName(prueba.c_str());
	prueba = "Assets/Models/" + prueba;

	uint bytesFile = App->file_system->Load(prueba.c_str(), &buffer);

	//Checks if buffer its empty or not and load file from that resource, if not we load from path
	if (buffer != nullptr)
		scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, NULL);
	else
		scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
		

	if (scene != nullptr && scene->HasMeshes()) {

		//aiNode* node = scene->mRootNode;

			for (size_t i = 0; i < scene->mNumMeshes; i++)
			{ 
				aiMesh* aiMesh = scene->mMeshes[i];
				//ourModel->meshes.push_back();
			}


	

		//App->importer->LoadNodes(scene, node, buffer, path);
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", path);


	RELEASE_ARRAY(buffer);

	LOG("Model took %d ms to be imported", modelImport.Read());
}

void ModelImporter::ImportNodes(const aiScene* scene, aiNode* node, char* fileBuffer, const char* path) {

	//Bring ImportManager.cpp -- > LoadNodes here
}

void ModelImporter::ImportTransformInfo(aiNode* node) {

	aiVector3D translation, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);
	//Todo: Save Into JSON scale rot translate for each node and generate localMatrix based on this values
}

uint64 ModelImporter::Save(const ResourceModel* ourModel, char** fileBuffer) {
	return -1;
}

GameObject* ModelImporter::Load(const char* fileBuffer, ResourceModel* ourModel) {

	GameObject* root = nullptr;

	//Create gameobjetstuff based on librarypaths for each json model. 

	/*
	* PSEUDO
	* READ JSON -> IF COMPONENT READ UID
	* SEARCH RESOURCE ATTACHED TO UID IN MAP
	* RETURN RESOURCE
	* MESHIMPORTER::LOAD(X,RESOURCE);
	* ROOT->CREATECOMPONENT(TYPEMESH, MESH....)
	*/


	return root;
}