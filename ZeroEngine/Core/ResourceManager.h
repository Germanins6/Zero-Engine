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
#include "ResourceMaterial.h"

#include "ZeroImporter.h"
#include "Serialization.h"

//Timestamp in UNIX
#include <ctime>

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
	Resource* ImportAssimpStuff(const char* path, ResourceType type, aiMesh* nodeMesh = nullptr, aiMaterial* nodeMaterial = nullptr);
	void SaveResource(Resource* resource);
	UID GenerateNewUID();

	bool CheckMetaFileExists(const char* assetsFile);
	void SaveMetaFile(Resource* resource);
	void LoadMetaFile(const char* path, ResourceType type);

	GameObject* SearchGameObjectByUID(UID id_to_search);
	Resource* RequestResource(UID id);
	void ReleaseResource(UID id);
	string GenLibraryPath(Resource* resource);

	//Path operations and string modifiers
	string SetPathFormated(UID uid_name, ResourceType fileType);
	PathInfo GetPathInfo(string path);
	ResourceType GetTypeByFormat(string format);

	Resource* CreateNewResource(const char* assetsFile, ResourceType type, bool fromFile = false, UID fromFileID = NULL);

private:

	map<UID, Resource*> resources;
	Serializer meta_file;
};