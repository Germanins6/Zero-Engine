#pragma once

#include "Module.h"

//Resource class
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"

typedef unsigned int UID;

using namespace std;

class ResourceManager : public Module {

public:

	ResourceManager(Application* app, bool start_enabled = true);
	~ResourceManager();

	UID Find(const char* file_in_assets) const;
	UID ImportFile(const char* new_file_in_assets);
	UID GenerateNewUID();

	const Resource* RequestResource(UID id) const;
	Resource* RequestResource(UID id);
	void ReleaseResource(UID id);

private:

	Resource* CreateNewResource(const char* assetsFile, ResourceType type);

private:

	map<UID, Resource*> resources;
};