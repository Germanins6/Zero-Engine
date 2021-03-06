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
	#pragma region Init_Import_Assets
	vector<string> ignoreAssetFiles;
	ignoreAssetFiles.push_back("meta");
	ignoreAssetFiles.push_back("ZeroScene");

	PathNode assets;
	assets = App->file_system->GetAllFiles("Assets", nullptr, &ignoreAssetFiles);

	CheckIfAssetsImported(assets);
	#pragma endregion Init_Import_Assets


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

				//If meta of any file doesnt exist import
				if (!App->file_system->Exists(metaPath.c_str()))
					App->resources->ImportFile(path.c_str());
			}
			CheckIfAssetsImported(node.children[i]);
		}
	}

}

string ResourceManager::LoadMetaFile(const char* path, ResourceType type) {

	string library_path;
	string metaPath(path);
	metaPath.append(".meta");

	switch (type)
	{
	case ResourceType::Model:
		ModelImporter::Model.Load(metaPath.c_str());
		library_path = ModelImporter::Model.GetString("LibraryPath");
		break;
	case ResourceType::Texture:
		TextureImporter::Texture.Load(metaPath.c_str());
		library_path = TextureImporter::Texture.GetString("LibraryPath");
		library_path = App->resources->GetPathInfo(library_path).name; //->This returns UID
	}

	return library_path;
}

void ResourceManager::LoadResource(const char* path, ResourceType type, UID id) {

	Resource* resource = nullptr;
	string libPath;

	switch (type) {
	case ResourceType::Mesh: 
		resource = App->resources->CreateNewResource(path, type, true, id);
		libPath.append(MESH_PATH);
		libPath.append(to_string(id));
		MeshImporter::Load(libPath.c_str(), dynamic_cast<ResourceMesh*>(resource));
		break;
	case ResourceType::Material: 
		resource = App->resources->CreateNewResource(path, type, true, id);
		libPath.append(MATERIAL_PATH);
		libPath.append(to_string(id));
		MaterialImporter::Load(libPath.c_str(), dynamic_cast<ResourceMaterial*>(resource));
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

Resource* ResourceManager::RequestResource(UID id, bool comparingPurpose) {

	Resource* resource = nullptr;

	map<UID, Resource*>::iterator it = resources.find(id);

	if (it != resources.end()) {
		if(it->second != nullptr)
			if(!comparingPurpose)it->second->referenceCount++;
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

	return ResourceType::None;
}

string ResourceManager::SetPathFormated(UID uid_name, ResourceType fileType) {

	string formattedPath;
	string uid = to_string(uid_name); // --> Convert UID for each resource into string to set name in library

	if (fileType == ResourceType::Mesh)
		formattedPath = MESH_PATH + uid;

	if (fileType == ResourceType::Texture)
		formattedPath = TEXTURE_PATH + uid;

	if (fileType == ResourceType::Model)
		formattedPath = MODEL_PATH + uid;

	if (fileType == ResourceType::Material)
		formattedPath = MATERIAL_PATH + uid ;
	
	return formattedPath;
}

void ResourceManager::DeleteAsset(const char* file) {

	string format = GetPathInfo(file).format.c_str();
	ResourceType type = GetTypeByFormat(format);
	string libPath;
	string metaPath(file);

	switch (type) {
	case ResourceType::Model:
		libPath = LoadMetaFile(file, type);
		DeleteModelResources(libPath.c_str());

		App->file_system->Remove(metaPath.append(".meta").c_str());
		break;
	case ResourceType::Texture:
		libPath = LoadMetaFile(file, type);
		App->file_system->Remove(App->resources->SetPathFormated(stoi(libPath), type).c_str()); //->Remove resource linked from Lib
		App->file_system->Remove(metaPath.append(".meta").c_str());
		break;
	}

	//Scene can be deleted by itself if no case
	App->file_system->Remove(file);
	
}

void ResourceManager::DeleteModelResources(const char* libPath) {
	
	ModelImporter::Model.Load(libPath);

	for (size_t i = 0; i <= Model.GetUnsignedInt("-Num_Children"); i++)
	{
		UID meshUID = Model.GetUnsignedIntObj("MeshUID", to_string(i));
		UID materialUID = Model.GetUnsignedIntObj("MaterialUID", to_string(i));


		if (meshUID != 0)
			App->file_system->Remove(App->resources->SetPathFormated(meshUID, ResourceType::Mesh).c_str()); //->Remove resource linked from Lib

		//Access material textures being used and delete path
		if (materialUID != 0) {
			if (App->file_system->Exists(SetPathFormated(materialUID, ResourceType::Material).c_str())) {
				MaterialImporter::Material.Load(SetPathFormated(materialUID, ResourceType::Material).c_str());
				App->file_system->Remove(MaterialImporter::Material.GetString("Diffuse_libraryPath").c_str());
				App->file_system->Remove(App->resources->SetPathFormated(materialUID, ResourceType::Material).c_str());
			}
		}

	}
}