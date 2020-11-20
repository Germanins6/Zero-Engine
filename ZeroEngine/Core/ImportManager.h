#pragma once

#include "Module.h"
#include <string>

#define MESH_PATH "Library/Meshes/"
#define TEXTURE_PATH "Library/Textures/"

//Forward declaration from ComponentMesh and Importers;
#include "ComponentMesh.h"
#include "ZeroImporter.h"

//Forward declaration
struct aiScene;
struct aiNode;

//Namespaces
using namespace MeshImporter;
using namespace TextureImporter;
using namespace std;

enum ImportType {
	ImportNull = 0,
	ImportMesh,
	ImportTexture,
	Max_Imports,
};

struct PathInfo {
	string path;
	string name;
	string format;
};

class ImportManager : public Module {

public:

	ImportManager(Application* app, bool start_enabled = true);
	~ImportManager();

	bool Init();
	bool CleanUp();

	//String modifiers
	string SetPathFormated(string desPathAppend, ImportType fileType);
	PathInfo GetPathInfo(string path);

	
	//Importing functions
	void ImportManager::ImportResource(string file_format);
	bool ImportManager::LoadGeometry(const char* path);
	GameObject* LoadNodes(const aiScene* scene, aiNode* node, char* fileBuffer, const char* path);

public:


};