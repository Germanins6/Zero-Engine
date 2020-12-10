#include "Application.h"
#include "ZeroImporter.h"

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

void MeshImporter::Load(const char* fileBuffer, ResourceMesh* ourMesh) {

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

void TextureImporter::Load(const char* fileBuffer, ResourceTexture* ourTexture) {

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

	//Store texture info into ResourceTexture values
	ourTexture->gpu_id = temp;
	ourTexture->height = ilGetInteger(IL_IMAGE_HEIGHT);
	ourTexture->width = ilGetInteger(IL_IMAGE_WIDTH);
	ourTexture->type = ilGetInteger(IL_IMAGE_FORMAT);
	ourTexture->data = ilGetData();

	//Create textures (diffuse,normal,specular...) into glBuffers
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, (GLuint*)&(ourTexture->gpu_id));
	glBindTexture(GL_TEXTURE_2D, ourTexture->gpu_id);

	//Filter parameters. TODO: Implement with Importing Options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Generate Image with info
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)ourTexture->GetWidth(), (int)ourTexture->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)ourTexture->data);
	
	//Unbind IL and GL
	glBindTexture(GL_TEXTURE_2D, 0);
	ilBindImage(0);

	LOG("Succesfully image loaded with: ID %u SIZE %u X %u", ourTexture->gpu_id, ourTexture->width, ourTexture->height);
	LOG("Image file took %d ms to be loaded", imageLoad.Read());
}

// ==== MODEL ==== //

void ModelImporter::Import(const char* path, ResourceModel* ourModel) {

	Timer modelImport;
	modelImport.Start();

	const aiScene* scene = nullptr;

	char* buffer;
	string resourcePath = App->file_system->NormalizePath(path);
	resourcePath = App->file_system->SetNormalName(resourcePath.c_str());
	resourcePath = "Assets/Models/" + resourcePath;

	uint bytesFile = App->file_system->Load(resourcePath.c_str(), &buffer);

	//Checks if buffer its empty or not and load file from that resource, if not we load from path
	if (buffer != nullptr)
		scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, NULL);
	else
		scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
		

	if (scene != nullptr && scene->HasMeshes()) {

		aiNode* node = scene->mRootNode;

		//Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{ 
			//Retrieve mesh data for each node
			aiMesh* aiMesh = scene->mMeshes[i];
			ResourceMesh* mesh = (ResourceMesh*)App->resources->ImportAssimpStuff(resourcePath.c_str(), ResourceType::Mesh, aiMesh);
			ourModel->meshes.push_back(mesh);

			App->resources->SaveResource(mesh);
		}

		//Use scene->mNumMaterials to iterate on scene->mMaterials array
		for (size_t i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* aiMaterial = scene->mMaterials[i];
			ResourceMaterial* material = (ResourceMaterial*)App->resources->ImportAssimpStuff(resourcePath.c_str(), ResourceType::Material, nullptr, aiMaterial);
			ourModel->materials.push_back(material);

			App->resources->SaveResource(material);
		}

		//Recursive function that will retrieve each node info stored into
		ModelImporter::ImportNodes(scene, node, ourModel);
		
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", path);


	RELEASE_ARRAY(buffer);

	LOG("Model took %d ms to be imported", modelImport.Read());
}

int ModelImporter::ImportNodes(const aiScene* scene, aiNode* node, ResourceModel* ourModel, int iterator, UID parentId) {

	//Bring ImportManager.cpp -- > LoadNodes here
	Model.AddUnsignedInt("-Num_Children", iterator);

	Model.Object[to_string(iterator)];
	Model.AddStringObj("Name", node->mName.C_Str(), to_string(iterator));
	UID rootUID = App->resources->GenerateNewUID();
	Model.AddUnsignedIntObj("ID", rootUID, to_string(iterator));
	Model.AddUnsignedIntObj("IDParent", parentId, to_string(iterator));
	ImportTransformInfo(node, iterator);

	//If actual node have a mesh we store uid value into json to be loaded later from our resource manager in Model::Load
	if (node->mMeshes != nullptr) {
		Model.AddStringObj("MeshUID", ourModel->meshes[*node->mMeshes]->GetLibraryFile(), to_string(iterator));
	}
	else{ 
		Model.AddStringObj("MeshUID", "0", to_string(iterator));
	}

	//Store material uids into respective gameObject using it
	if (node->mMeshes != nullptr)
		Model.AddStringObj("MaterialUID", ourModel->materials[scene->mMeshes[*node->mMeshes]->mMaterialIndex]->GetLibraryFile(), to_string(iterator));
	else
		Model.AddStringObj("MaterialUID", "0", to_string(iterator));

	//ERROR FOUND- TODO: FIX MISTAKE, ALL MESHES DOES HAVE MATERIAL BUT MAYBE CANNOT HAVE ATTACHED DIFFUSE IMAGE THEN DONT CREATE COMPONENT OR ZEROMAT WITH DIFFUSE UID 0

	//Iterates each child, stores its info into root child vector, and save parent info for each child recursively
	if (node->mNumChildren > 0)
		for (int i = 0; i < node->mNumChildren; ++i)
			iterator = ImportNodes(scene, node->mChildren[i], ourModel, ++iterator, rootUID);

	return iterator;
}

void ModelImporter::ImportTransformInfo(aiNode* node, int iterator) {

	aiVector3D translation, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	Model.AddFloat3Obj("Translation", { translation.x, translation.y , translation.z }, to_string(iterator));
	Model.AddQuaternionObj("Rotation", { rotation.x, rotation.y, rotation.z , rotation.w}, to_string(iterator));
	Model.AddFloat3Obj("Scale", { scaling.x, scaling.y, scaling.z }, to_string(iterator));
}

uint64 ModelImporter::Save(const ResourceModel* ourModel) {

	Model.Save(ourModel->libraryFile.c_str());
	return -1;
}

void ModelImporter::Load(const char* fileBuffer) {

	//Open Model file from Lib
	Model.Load(fileBuffer);
	
	//By default constructor creates transform
	for (size_t i = 0; i <= Model.GetUnsignedInt("-Num_Children"); i++)
	{
		GameObject* gameObject = new GameObject();

		gameObject->name = Model.GetStringObj("Name", to_string(i));
		gameObject->uuid = Model.GetUnsignedIntObj("ID", to_string(i));
		gameObject->parentId = Model.GetUnsignedIntObj("IDParent", to_string(i));

		//Parent child to parent and parent to child
		if (gameObject->parentId != 0) {
			gameObject->parent = App->resources->SearchGameObjectByUID(gameObject->parentId);
			App->resources->SearchGameObjectByUID(gameObject->parentId)->children.push_back(gameObject);
		}

		//Transform
		float3 translate = Model.GetFloatXYZObj("Translation", to_string(i));
		Quat rotation = Model.GetQuaternionObj("Rotation", to_string(i));
		float3 scale = Model.GetFloatXYZObj("Scale", to_string(i));

		ComponentTransform* transform = dynamic_cast<ComponentTransform*>(gameObject->GetTransform());
		transform->SetPosition(translate.x, translate.y, translate.z);
		transform->euler = rotation.ToEulerXYZ() * RADTODEG;
		transform->SetRotation(transform->euler.x, transform->euler.y, transform->euler.z);
		transform->SetScale(scale.x, scale.y, scale.z);
		transform->UpdateGlobalMatrix();

		//Mesh
		string meshUID = Model.GetStringObj("MeshUID", to_string(i)); //Returns path but not just UID
		UID fileUID = stoi(App->resources->GetPathInfo(meshUID).name);
		if (meshUID != "0") {
			ResourceMesh* newMesh = dynamic_cast<ResourceMesh*>(App->resources->CreateNewResource("Remember store asset path", ResourceType::Mesh, true, fileUID));
			MeshImporter::Load(meshUID.c_str(), newMesh);
			gameObject->CreateComponent(ComponentType::MESH, fileUID);
		}

		//Texture
		string materialUID = Model.GetStringObj("MaterialUID", to_string(i));
		fileUID = stoi(App->resources->GetPathInfo(materialUID).name);
		if (materialUID != "0") {
			ResourceMaterial* newMaterial = dynamic_cast<ResourceMaterial*>(App->resources->CreateNewResource("Remember store asset path", ResourceType::Material, true, fileUID));
			MaterialImporter::Load(materialUID.c_str(), newMaterial);
			gameObject->CreateComponent(ComponentType::MATERIAL, fileUID);
		}

		App->scene->gameobjects.push_back(gameObject);

	}

}

// ==== MATERIAL ==== //

void MaterialImporter::Import(const aiMaterial* aiMaterial, ResourceMaterial* ourMaterial) {
	Timer materialImport;
	materialImport.Start();

	if (aiMaterial != nullptr) {

		//Get texture path info from node
		aiString texture_path;
		aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);

		aiColor4D diffuseRGBA;
		aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseRGBA);

		string normalizedPath = texture_path.C_Str();
		normalizedPath = normalizedPath.substr(normalizedPath.find_last_of(0x5c) + 1);

		//Store always color diffuse info independently if any mesh does have texture or not.
		ourMaterial->materialColor.r = diffuseRGBA.r;
		ourMaterial->materialColor.g = diffuseRGBA.g;
		ourMaterial->materialColor.b = diffuseRGBA.b;
		ourMaterial->materialColor.a = diffuseRGBA.a;

		//Store and import image files if any material use it as diffuse.
		if (normalizedPath.size() > 0) {
			string real_path = "Assets/Textures/";
			string str_texture(normalizedPath.c_str());
			real_path += str_texture;

			//If the current file actually exists doesnt need to be imported once again
			if (!App->resources->CheckMetaFileExists(real_path.c_str())) {
				ourMaterial->diffuse_id = App->resources->ImportFile(real_path.c_str());
			}
			else {
				TextureImporter::Texture.Load(real_path.append(".meta").c_str());
				string LibPath = TextureImporter::Texture.GetString("LibraryPath");
				ourMaterial->diffuse_id = stoi(App->resources->GetPathInfo(LibPath).name);
			}
		}
	}

	LOG("Material took %d ms to be imported", materialImport.Read());
}

uint64 MaterialImporter::Save(ResourceMaterial* ourMaterial) {

	//Add info into json file and save in Library
	Material.AddUnsignedInt("Diffuse", ourMaterial->diffuse_id);
	Material.AddFloat("R", ourMaterial->materialColor.r);
	Material.AddFloat("G", ourMaterial->materialColor.g);
	Material.AddFloat("B", ourMaterial->materialColor.b);
	Material.AddFloat("A", ourMaterial->materialColor.a);

	Material.Save(ourMaterial->libraryFile.c_str());
	return -1;
}

void MaterialImporter::Load(const char* fileBuffer, ResourceMaterial* ourMaterial) {
	Timer materialLoad;
	materialLoad.Start();

	//Open file with json and retrieves uid from Diffuse channel
	Material.Load(fileBuffer);

	ourMaterial->materialColor.r = Material.GetFloat("R");
	ourMaterial->materialColor.g = Material.GetFloat("G");
	ourMaterial->materialColor.b = Material.GetFloat("B");
	ourMaterial->materialColor.a = Material.GetFloat("A");

	UID fileUID = Material.GetUnsignedInt("Diffuse");
	if (fileUID != 0) {
		ourMaterial->diffuse = dynamic_cast<ResourceTexture*>(App->resources->CreateNewResource("Remember store texture path", ResourceType::Texture, true, fileUID));
		ourMaterial->diffuse_id = fileUID;
		TextureImporter::Load(ourMaterial->diffuse->libraryFile.c_str(), ourMaterial->diffuse);
	}

	LOG("Material took %d ms to be loaded", materialLoad.Read());
}