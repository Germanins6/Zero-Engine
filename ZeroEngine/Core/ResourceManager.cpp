#include "Application.h"
#include "ResourceManager.h"

//Include to generate random numbers from MathGeoLib
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled) {

}

ResourceManager::~ResourceManager() {

}

bool ResourceManager::Init() {
	MeshImporter::Init();
	TextureImporter::Init();

	return true;
}

bool ResourceManager::CleanUp() {

	MeshImporter::CleanUp();
	TextureImporter::CleanUp();

	return true;
}

UID ResourceManager::Find(const char* file_in_assets) const {
	
	UID id = 0;


	return id;
}

UID ResourceManager::ImportFile(const char* path) {

	UID id = 0;

	//Get file format and creates resource based on: fbx->model png/tga... -> texture
	string file_format = GetPathInfo(path).format;
	Resource* resource = CreateNewResource(path, GetTypeByFormat(file_format));

	//Import depending path given
	switch (resource->type) {
	case ResourceType::Model: ModelImporter::Import(path , (ResourceModel*)(resource)); break;
	case ResourceType::Texture: TextureImporter::Import(path); break;
	}

	SaveResource(resource);
	id = resource->GetUID();
	ReleaseResource(id);

	return id;
}

void ResourceManager::SaveResource(Resource* resource) {

	char* bufferFile = nullptr;
	uint size = 0;

	switch (resource->GetType()) {
	case ResourceType::Mesh:	
		size = MeshImporter::Save((ResourceMesh*)resource, &bufferFile);
		break;
	case ResourceType::Texture:
		size = TextureImporter::Save(&bufferFile);
		break;
	case ResourceType::Model:
		size = ModelImporter::Save((ResourceModel*)resource, &bufferFile);
		break;
	}

	/*
	*GENERATE META AFTER IMPORT INTO ASSETS 
	*/

	//Save resource into our library folder
	App->file_system->Save(resource->libraryFile.c_str(), bufferFile, size);
	RELEASE_ARRAY(bufferFile);
}

UID ResourceManager::GenerateNewUID() {

	LCG randomNumber;
	return randomNumber.Int();

}

Resource* ResourceManager::RequestResource(UID id) {

	Resource* resource = nullptr;

	map<UID, Resource*>::iterator it = resources.find(id);

	if (it != resources.end()) {
		it->second->referenceCount++;
		return it->second;
	}

	return resource;
}

void ResourceManager::ReleaseResource(UID id) {
	RELEASE(resources[id]);
}

Resource* ResourceManager::CreateNewResource(const char* assetsPath, ResourceType type) {

	Resource* resource = nullptr;

	UID id = GenerateNewUID();

	switch (type) {
	case ResourceType::Mesh: resource = dynamic_cast<Resource*>(new ResourceMesh(id)); break;
	case ResourceType::Texture: resource = dynamic_cast<Resource*>(new ResourceTexture(id)); break;
	case ResourceType::Model: resource = dynamic_cast<Resource*>(new ResourceModel(id)); break;
	}

	if (resource != nullptr) {
		resources.insert({ id, resource });
		resource->assetsFile = assetsPath;
		//resource->libraryFile = GenLibraryPath(resource); // This method the same that i have into import manager
	}

	return resource;
}


PathInfo ResourceManager::GetPathInfo(string path) {

	PathInfo pathInfo;
	App->file_system->SplitFilePath(path.c_str(), &pathInfo.path, &pathInfo.name, &pathInfo.format);
	return pathInfo;
}

ResourceType ResourceManager::GetTypeByFormat(string file_format) {

	if (file_format == "fbx" || file_format == "obj")
		return ResourceType::Model;

	//Directly process textures info
	if (file_format == "png" || file_format == "jpg" || file_format == "tga")
		return ResourceType::Texture;

	return ResourceType::None;
}

string ResourceManager::SetPathFormated(UID uid_name, ResourceType fileType) {

	string formattedPath;
	string uid = to_string(uid_name); // --> Convert UID for each resource into string to set name in library

	if (fileType == ResourceType::Mesh) {
		string format = ".Zero";
		formattedPath = MESH_PATH + uid + format;
	}

	if (fileType == ResourceType::Texture) {
		string format = ".DDS";
		formattedPath = TEXTURE_PATH + uid + format;
	}

	if (fileType == ResourceType::Model) {
		string format = ".ZeroModel";
		formattedPath = MODEL_PATH + uid + format;
	}

	if (fileType == ResourceType::Scene) {
		string format = ".ZeroScene";
		formattedPath = SCENE_PATH + uid + format;
	}

	return formattedPath;
}