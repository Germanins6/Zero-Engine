#pragma once

#include "p2Defs.h"
#include "Module.h"

//Resource class
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"

#define MESH_PATH "Library/Meshes/"
#define TEXTURE_PATH "Library/Textures/"

using namespace std;

struct PathInfo {
	string path;
	string name;
	string format;
};

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

	//Path operations and string modifiers
	string SetPathFormated(string desPathAppend, ResourceType fileType);
	PathInfo GetPathInfo(string path);
	ResourceType GetTypeByFormat(string format);

private:

	Resource* CreateNewResource(const char* assetsFile, ResourceType type);

private:

	map<UID, Resource*> resources;
};