#include "ImportManager.h"

ImportManager::ImportManager(Application* app, bool start_enabled) : Module(app, start_enabled) {


}


ImportManager::~ImportManager() {


}

string ImportManager::SetPathFormated(string destPathAppend, ImportType fileType) {

	string formattedPath;

	if (fileType == ImportType::ImportMesh) {

		string path = "Library/Meshes/";
		string format = ".Zero";
		formattedPath = path + destPathAppend + format;
	}


	return formattedPath;
}