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

const Resource* ResourceManager::RequestResource(UID id)const {

	Resource* resource = nullptr;

	map<UID, Resource*>::iterator it = resources.find(id);

	if (it != resources.end()) {

	}

	return resource;
}
