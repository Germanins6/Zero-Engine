#pragma once

#include "Module.h"
#include <string>

#define MESH_PATH "Library/Meshes/"
#define TEXTURE_PATH "Library/Textures/"

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

	//String modifiers
	string SetPathFormated(string desPathAppend, ImportType fileType);
	PathInfo GetPathInfo(string path);

	
	//Importing functions
	void ImportManager::ImportResource(string file_format);

public:


};