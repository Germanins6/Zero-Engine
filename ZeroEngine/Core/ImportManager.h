#pragma once

#include "Module.h"
#include <string>

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

	string SetPathFormated(string desPathAppend, ImportType fileType);

public:


};