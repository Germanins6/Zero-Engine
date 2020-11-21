#include "ResourceManager.h"

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

	UID id = 0;

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
	case ResourceType::Mesh: resource = (Resource*) new ResourceMesh(id); break;
	case ResourceType::Texture: resource = (Resource*) new ResourceTexture(id); break;
	}

	if (resource != nullptr) {
		resources.insert({ id, resource });
		resource->assetsFile = assetsPath;
		resource->libraryFile = GenLibraryPath(resource); // This method the same that i have into import manager
	}

	return resource;
}