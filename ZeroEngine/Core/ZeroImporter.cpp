#include "ZeroImporter.h"

#include "Application.h"

#include "ComponentMesh.h"

//-- Assimp
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/mesh.h"

// -- DevIL Image Library
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#include "glew/include/glew.h"

//Devil Libs loading
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

void MeshImporter::Import(const aiMesh* aiMesh, Mesh* ourMesh){

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

uint64 MeshImporter::Save(const Mesh* ourMesh, char** fileBuffer) {

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
	bytes = sizeof(uint) * ourMesh->num_vertex * 3;
	ourMesh->vertex = new float[ourMesh->num_vertex * 3];
	memcpy(ourMesh->vertex, cursor, bytes);
	cursor += bytes;

	//Normals
	bytes = sizeof(uint) * ourMesh->num_normals * 3;
	ourMesh->normals = new float[ourMesh->num_normals * 3];
	memcpy(ourMesh->normals, cursor, bytes);
	cursor += bytes;

	//Uvs
	bytes = sizeof(uint) * ourMesh->num_uvs * 2;
	ourMesh->uv_coords = new float[ourMesh->num_uvs * 2];
	memcpy(ourMesh->uv_coords, cursor, bytes);
	cursor += bytes;

	meshes.push_back(ourMesh);

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
	for (size_t i = 0; i < textures.size(); i++)
		RELEASE(textures[i])

		textures.clear();
}

void TextureImporter::Import(char* BufferFile, Texture* ourTexture, uint bytesFile) {

	Timer imageImport;
	imageImport.Start();

	ILuint temp = 0;
	ilGenImages(1, &temp);
	ilBindImage(temp);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	//Create path buffer and import to scene
	char* buffer = nullptr;
	uint bytesFile = 0;
	string norm_path_short;
	if (buffer == nullptr) {
		norm_path_short = "Assets/Textures/" + App->file_system->SetNormalName(pathFile);
		bytesFile = App->file_system->Load(norm_path_short.c_str(), &buffer);
	}

	string extension(pathFile);
	ILenum type = IL_TYPE_UNKNOWN;
	extension = extension.substr(extension.find_last_of("."));

	if (extension == ".png")
		type = IL_PNG;
	else if (extension == ".jpg")
		type = IL_JPG;
	else if (extension == ".tga")
		type = IL_TGA;


	if (type != IL_TYPE_UNKNOWN && buffer != nullptr) {
		if (ilLoadL(type, buffer, bytesFile) == IL_FALSE) {
			if (ilLoadImage(norm_path_short.c_str()) == IL_FALSE)
				LOG("Source image from %s path Loaded Succesfully", norm_path_short.c_str())
			else
				LOG("Unable to load texture");
		}
	}

	LOG("Source image from %s path Loaded Succesfully", norm_path_short.c_str())
	
	//Initialitizing texture values and buff
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	ourTexture->id = temp;
	ourTexture->height = ilGetInteger(IL_IMAGE_HEIGHT);
	ourTexture->width = ilGetInteger(IL_IMAGE_WIDTH);
	ourTexture->type = ilGetInteger(IL_IMAGE_FORMAT);
	ourTexture->data = ilGetData();
	
	ilBindImage(0);

	LOG("Succesfully image loaded with: ID %u SIZE %u X %u", ourTexture->id, ourTexture->width, ourTexture->height);
	LOG("Image file took %d ms to be imported", imageImport.Read());
	RELEASE_ARRAY(buffer);
}

uint64 TextureImporter::Save(Texture* ourTexture, char** fileBuffer) {

	ILuint size;

	//Data must be ourTexture->data(?)
	ILubyte* data;

	ilBindImage(ourTexture->id);

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, nullptr, 0);

	if (size > 0) {

		data = new ILubyte[size];
		if (ilSaveL(IL_DDS, data, size) > 0)
			*fileBuffer = (char*)data;
		RELEASE(data);
	}

	ilBindImage(0);

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

	textures.push_back(ourTexture);

	ilBindImage(0);

	LOG("Succesfully image loaded with: ID %u SIZE %u X %u", ourTexture->id, ourTexture->width, ourTexture->height);
	LOG("Image file took %d ms to be imported", imageLoad.Read());
}

// === MATERIAL === //