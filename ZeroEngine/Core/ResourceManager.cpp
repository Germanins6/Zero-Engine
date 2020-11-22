#include "Application.h"

#include "ResourceManager.h"

#include "ZeroImporter.h"
using namespace MeshImporter;
using namespace TextureImporter;

//Include to generate random numbers from MathGeoLib
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled) {

}

ResourceManager::~ResourceManager() {

}

UID ResourceManager::Find(const char* file_in_assets) const {
	
	UID id = 0;


	return id;
}

UID ResourceManager::ImportFile(const char* new_file_in_assets) {

	//First we must get extension and create resource depending wich file weve loaded
	string extension = GetPathInfo(new_file_in_assets).format;
	Resource* resource = CreateNewResource(new_file_in_assets, GetTypeByFormat(extension));

	UID id = 0;

	switch (resource->type) {
	case ResourceType::Mesh: App->importer->LoadGeometry(new_file_in_assets); break;
	case ResourceType::Texture: App->importer->LoadTexture(new_file_in_assets); break;
	}





	return id;
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


Resource* ResourceManager::CreateNewResource(const char* assetsPath, ResourceType type) {

	Resource* resource = nullptr;

	UID id = GenerateNewUID();

	switch (type) {
	case ResourceType::Mesh: resource = dynamic_cast<Resource*>(new ResourceMesh(id)); break;
	case ResourceType::Texture: resource = dynamic_cast<Resource*>(new ResourceTexture(id)); break;
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
		return ResourceType::Mesh;
	
	if (file_format == "png" || file_format == "jpg" || file_format == "tga")
		return ResourceType::Texture;

	return ResourceType::None;
}