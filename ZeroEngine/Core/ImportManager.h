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

class ImportManager : public Module {

public:

	ImportManager(Application* app, bool start_enabled = true);
	~ImportManager();

	bool Init();
	bool CleanUp();

	//String modifiers
	string SetPathFormated(string desPathAppend, ImportType fileType);
	
	//Importing functions
	bool ImportManager::LoadGeometry(const char* path);
	GameObject* LoadNodes(const aiScene* scene, aiNode* node, char* fileBuffer, const char* path);

	void LoadTexture(const char* path);

public:


};