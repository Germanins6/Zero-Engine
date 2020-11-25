#pragma once

#include "p2Defs.h"
#include "Module.h"

//Tools
#include <string>
#include <vector>

//Resource class
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ResourceModel.h"

#include "ZeroImporter.h"

//Namespaces
using namespace MeshImporter;
using namespace TextureImporter;
using namespace ModelImporter;
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
	bool Init();
	bool CleanUp();

	UID Find(const char* file_in_assets) const;
	UID ImportFile(const char* new_file_in_assets);
	void SaveResource(Resource* resource);
	UID GenerateNewUID();

	const Resource* RequestResource(UID id) const;
	Resource* RequestResource(UID id);
	void ReleaseResource(UID id);

	//Path operations and string modifiers
	string SetPathFormated(UID uid_name, ResourceType fileType);
	PathInfo GetPathInfo(string path);
	ResourceType GetTypeByFormat(string format);

	Resource* CreateNewResource(const char* assetsFile, ResourceType type);

private:

	map<UID, Resource*> resources;
};