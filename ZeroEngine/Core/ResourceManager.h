#pragma once

#include "Module.h"

//Resource class
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"

class ResourceManager : public Module {

public:

	ResourceManager(Application* app, bool start_enabled = true);
	~ResourceManager();

	uint Find(const char* file_in_assets) const;
	uint ImportFile(const char* new_file_in_assets);
	uint GenerateNewUID();

	const Resource* RequestResource(uint id) const;
	Resource* RequestResource(uint id);
	void ReleaseResource(uint id);

private:

	Resource* CreateNewResource(const char* assetsFile, ResourceType type);

private:

	std::map<uint, Resource*> resources;
};