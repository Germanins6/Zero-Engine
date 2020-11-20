
#include "ImportManager.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#include "ZeroImporter.h"

ImportManager::ImportManager(Application* app, bool start_enabled) : Module(app, start_enabled) {


}


ImportManager::~ImportManager() {


}

PathInfo ImportManager::GetPathInfo(string path) {

	PathInfo pathInfo;
	App->file_system->SplitFilePath(path.c_str(), &pathInfo.path, &pathInfo.name, &pathInfo.format);
	return pathInfo;
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

void ImportManager::ImportResource(string path) {

	//Retrieve extension
	string file_format = GetPathInfo(path).format;

	//Process mesh info
	if (file_format == ".fbx" || file_format == ".obj") {


	}

	//Directly process textures info
	if (file_format == ".png" || file_format == ".jpg" || file_format == "tga") {
		TextureImporter::Import(path.c_str());
		char* buffer = nullptr;
		uint size = TextureImporter::Save(&buffer);
		App->file_system->Save(GetPathInfo(path).name.c_str(), buffer, size);
	}
}