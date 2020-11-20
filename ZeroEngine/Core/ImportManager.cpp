
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
		string format = ".Zero";
		formattedPath = MESH_PATH + destPathAppend + format;
	}

	if (fileType == ImportType::ImportTexture) {
		string format = ".DDS";
		formattedPath = TEXTURE_PATH + destPathAppend + format;
	}


	return formattedPath;
}

void ImportManager::ImportResource(string path) {

	//Retrieve extension
	string file_format = GetPathInfo(path).format;

	//Process mesh info
	if (file_format == "fbx" || file_format == "obj") {

		App->geometry->LoadGeometry(path.c_str()); //think about deleting this module and refactor code into importManager module instead having 2 same doing stuff modules
	}

	//Directly process textures info
	if (file_format == "png" || file_format == "jpg" || file_format == "tga") {
		TextureImporter::Import(path.c_str());
		char* buffer = nullptr;
		uint size = TextureImporter::Save(&buffer);
		App->file_system->Save(GetPathInfo(path).name.c_str(), buffer, size);
	}
}