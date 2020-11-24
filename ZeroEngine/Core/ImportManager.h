#pragma once

#include "Module.h"
#include <string>


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



class ImportManager : public Module {

public:

	ImportManager(Application* app, bool start_enabled = true);
	~ImportManager();

	bool Init();
	bool CleanUp();
	

	
	//Importing functions
	void ImportManager::ImportResource(string file_format);
	bool ImportManager::LoadGeometry(const char* path);
	GameObject* LoadNodes(const aiScene* scene, aiNode* node, char* fileBuffer, const char* path);
	void GetTransformInfo(aiNode* node);

public:


};