#include "Application.h"
#include "ResourceManager.h"

//Include to generate random numbers from MathGeoLib
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled) {

}

ResourceManager::~ResourceManager() {

	//Delete resources still loaded in memory
	map<UID, Resource*>::iterator it = resources.begin();

	while (it != resources.end())
		ReleaseResource(it->first);

	LOG("Destroying stuff");

	resources.clear();

}

bool ResourceManager::Init() {

	MeshImporter::Init();
	TextureImporter::Init();

	//Loading assets pathNode to check non imported new files into our folder
	vector<string> ignoreAssetFiles;
	ignoreAssetFiles.push_back("meta");
	ignoreAssetFiles.push_back("ZeroScene");

	PathNode assets;
	assets = App->file_system->GetAllFiles("Assets", nullptr, &ignoreAssetFiles);

	CheckIfAssetsImported(assets);

	//Loading all our resources to be used later at least once
	vector<string> ignoreLibraryFiles;
	ignoreLibraryFiles.push_back("dds");

	PathNode library;
	library = App->file_system->GetAllFiles("Library", nullptr, nullptr);
	InitResources(library);

	return true;
}

bool ResourceManager::CleanUp() {

	MeshImporter::CleanUp();
	TextureImporter::CleanUp();

	return true;
}

UID ResourceManager::Find(const char* file_in_assets) const {
	
	UID id = 0;


	return id;
}

UID ResourceManager::ImportFile(const char* path) {

	UID id = 0;

	//Get file format and creates resource based on: fbx->model png/tga... -> texture
	string file_format = GetPathInfo(path).format;
	Resource* resource = CreateNewResource(path, GetTypeByFormat(file_format));

	resource->assetsFile = App->file_system->GetPathRelativeToAssets(App->file_system->NormalizePath(resource->assetsFile.c_str()).c_str());
	//Import depending path given
	switch (resource->type) {
	case ResourceType::Model: ModelImporter::Import(resource->assetsFile.c_str(), (ResourceModel*)(resource)); break;
	case ResourceType::Texture: TextureImporter::Import(resource->assetsFile.c_str()); break;
	}

	SaveResource(resource);
	id = resource->GetUID();

	SaveMetaFile(resource);

	ReleaseResource(id);

	return id;
}

Resource* ResourceManager::ImportAssimpStuff(const char* path, ResourceType type, aiMesh* nodeMesh, aiMaterial* nodeMaterial) {

	UID id;

	Resource* resource = CreateNewResource(path, type);

	switch (type) {
	case ResourceType::Mesh: MeshImporter::Import(nodeMesh, dynamic_cast<ResourceMesh*>(resource)); break;
	case ResourceType::Material: MaterialImporter::Import(nodeMaterial, dynamic_cast<ResourceMaterial*>(resource)); break;
	}

	return resource;
}

bool ResourceManager::CheckMetaFileExists(const char* assetsFile) {

	//Checks if file dropped does have meta file and if need to be imported once again or not
	bool exists = false;

	string metaPath(App->file_system->NormalizePath(assetsFile));
	metaPath.append(".meta");
	metaPath = App->file_system->GetPathRelativeToAssets(metaPath.c_str());

	exists = App->file_system->Exists(metaPath.c_str());
	
	return exists;
}

void ResourceManager::CheckIfAssetsImported(PathNode node) {

	if (node.children.size() > 0) {
		for (size_t i = 0; i < node.children.size(); i++)
		{
			if (node.children[i].isFile) {
				string path = node.children[i].path;
				string metaPath(path);
				metaPath.append(".meta");

				if (App->file_system->Exists(metaPath.c_str()))
					LOG("DONT IMPORT")
				else
					App->resources->ImportFile(path.c_str());
			}
			CheckIfAssetsImported(node.children[i]);
		}
	}

}

void ResourceManager::InitResources(PathNode node, ResourceType fileType) {

	ResourceType type = fileType;
	string localPath;

	if (node.children.size() > 0) {
		for (size_t i = 0; i < node.children.size(); i++)
		{
			localPath = node.children[i].localPath.c_str();

			//Check wich folder we are inside and store files type we are going to import next recursive file node calls
			if (!node.children[i].isFile) {
				
				// this would be Library/Textures/24332532535 non formated file to avoid string operations
				string libFolder = node.children[i].path; 
				
				if (libFolder == "Library/Textures") type = ResourceType::Texture;
				else if (libFolder == "Library/Meshes") type = ResourceType::Mesh;
				else if (libFolder == "Library/Materials") type = ResourceType::Material;
				else if (libFolder == "Library/Models") type = ResourceType::Model;
			}
			
			//If we are in Library/Materials we instantly go through folder files
			InitResources(node.children[i], type);
			
			//Checks we find a file and will load resource into memory depending wich fileType we are loading
			if (isdigit(localPath[0])) {
				LOG("%s", node.children[i].path.c_str());

				Resource* resource = nullptr;

				switch (fileType) {
				case ResourceType::Texture: 
					resource = App->resources->CreateNewResource("IDK", ResourceType::Texture, true, stoi(node.children[i].localPath));
					TextureImporter::Load(node.children[i].path.c_str(), dynamic_cast<ResourceTexture*>(resource));
					break;
				case ResourceType::Mesh: 
					resource = App->resources->CreateNewResource("IDK", ResourceType::Mesh, true, stoi(node.children[i].localPath));
					MeshImporter::Load(node.children[i].path.c_str(), dynamic_cast<ResourceMesh*>(resource));
					break;
				case ResourceType::Material: 
					resource = App->resources->CreateNewResource("IDK", ResourceType::Material, true, stoi(node.children[i].localPath));
					MaterialImporter::Load(node.children[i].path.c_str(), dynamic_cast<ResourceMaterial*>(resource));
					break;
				}
			}
		}
	}
}

void ResourceManager::LoadMetaFile(const char* path, ResourceType type) {

	string library_path;

	string metaPath(path);
	metaPath.append(".meta");

	switch (type)
	{
	case ResourceType::Model:
		ModelImporter::Model.Load(metaPath.c_str());
		library_path = ModelImporter::Model.GetString("LibraryPath");
		ModelImporter::Load(library_path.c_str());
		break;
	case ResourceType::Texture:
		break;
	case ResourceType::Scene:
		break;
	case ResourceType::None:
		break;

	}

}

void ResourceManager::SaveMetaFile(Resource* resource) {

	meta_file.AddUnsignedInt("Time_LastModified", App->file_system->GetLastModTime(resource->assetsFile.c_str()));
	meta_file.AddUnsignedInt("Time_MetaCreated", time(0));
	meta_file.AddUnsignedInt("UID", resource->GetUID());
	meta_file.AddString("LibraryPath", resource->GetLibraryFile());
	
	//Setting meta extension to each file in their proper directory
	string metaName(resource->GetAssetFile());
	metaName += ".meta";
	meta_file.Save(metaName.c_str());
}

void ResourceManager::SaveResource(Resource* resource) {

	char* bufferFile = nullptr;
	uint size = 0;

	switch (resource->GetType()) {
	case ResourceType::Mesh:	
		size = MeshImporter::Save((ResourceMesh*)resource, &bufferFile);
		break;
	case ResourceType::Texture:
		size = TextureImporter::Save(&bufferFile);
		break;
	case ResourceType::Model:
		size = ModelImporter::Save((ResourceModel*)resource);
		break;
	case 
		ResourceType::Material:
		size = MaterialImporter::Save((ResourceMaterial*)resource);
		break;
	}

	//Save resource into our library folder
	if(size != -1)
		App->file_system->Save(resource->libraryFile.c_str(), bufferFile, size);

	RELEASE_ARRAY(bufferFile);
}

UID ResourceManager::GenerateNewUID() {

	LCG randomNumber;
	return randomNumber.Int();

}

GameObject* ResourceManager::SearchGameObjectByUID(UID id_to_search) {

	for (size_t i = 0; i < App->scene->gameobjects.size(); i++)
	{
		if (App->scene->gameobjects[i]->uuid == id_to_search)
			return App->scene->gameobjects[i];
	}

	return nullptr;
}

Resource* ResourceManager::RequestResource(UID id) {

	Resource* resource = nullptr;

	map<UID, Resource*>::iterator it = resources.find(id);

	if (it != resources.end()) {
		it->second->referenceCount++;
		return it->second;
	}

	return resource;
}

void ResourceManager::ReleaseResource(UID id) {
	RELEASE(resources[id]);
}

Resource* ResourceManager::CreateNewResource(const char* assetsPath, ResourceType type, bool fromFile, UID fromFileID) {

	Resource* resource = nullptr;
	UID id = 0;

	//If function called to load file store UID value, if first time creating resource then create new one
	fromFile ? id = fromFileID : id = GenerateNewUID();
	
	switch (type) {
	case ResourceType::Mesh: resource = dynamic_cast<Resource*>(new ResourceMesh(id)); break;
	case ResourceType::Texture: resource = dynamic_cast<Resource*>(new ResourceTexture(id)); break;
	case ResourceType::Model: resource = dynamic_cast<Resource*>(new ResourceModel(id)); break;
	case ResourceType::Material: resource = dynamic_cast<Resource*>(new ResourceMaterial(id)); break;
	}

	if (resource != nullptr) {
		resources.insert({ id, resource });
		resource->assetsFile = App->file_system->NormalizePath(assetsPath);
		resource->libraryFile = GenLibraryPath(resource); // This method the same that i have into import manager
	}

	return resource;
}

string ResourceManager::GenLibraryPath(Resource* resource) {
	
	string libPath;
	string uidName(to_string(resource->GetUID()));

	switch(resource->type) {
	case ResourceType::Model: libPath = MODEL_PATH + uidName; break;
	case ResourceType::Mesh: libPath = MESH_PATH + uidName; break;
	case ResourceType::Texture: libPath = TEXTURE_PATH + uidName; break;
	case ResourceType::Material: libPath = MATERIAL_PATH + uidName; break;
	}


	return libPath;
}


PathInfo ResourceManager::GetPathInfo(string path) {

	PathInfo pathInfo;
	App->file_system->SplitFilePath(path.c_str(), &pathInfo.path, &pathInfo.name, &pathInfo.format);
	return pathInfo;
}

ResourceType ResourceManager::GetTypeByFormat(string file_format) {

	if (file_format == "fbx" || file_format == "obj" || file_format == "FBX")
		return ResourceType::Model;

	if (file_format == "png" || file_format == "jpg" || file_format == "tga")
		return ResourceType::Texture;

	if (file_format == "ZeroScene")
		return ResourceType::Scene;

	if (file_format == "ZeroMaterial")
		return ResourceType::Material;

	return ResourceType::None;
}

string ResourceManager::SetPathFormated(UID uid_name, ResourceType fileType) {

	string formattedPath;
	string uid = to_string(uid_name); // --> Convert UID for each resource into string to set name in library

	if (fileType == ResourceType::Mesh) {
		string format = ".Zero";
		formattedPath = MESH_PATH + uid + format;
	}

	if (fileType == ResourceType::Texture) {
		string format = ".DDS";
		formattedPath = TEXTURE_PATH + uid + format;
	}

	if (fileType == ResourceType::Model) {
		string format = ".ZeroModel";
		formattedPath = MODEL_PATH + uid + format;
	}

	if (fileType == ResourceType::Scene) {
		string format = ".ZeroScene";
		formattedPath = SCENE_PATH + uid + format;
	}

	if (fileType == ResourceType::Material) {
		string format = ".ZeroMaterial";
		formattedPath = MATERIAL_PATH + uid + format;
	}

	return formattedPath;
}