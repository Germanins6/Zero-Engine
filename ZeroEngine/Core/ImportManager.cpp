#include "ImportManager.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

// -- Tools
#include <vector>
#include "SDL/include/SDL_opengl.h"

//-- Assimp
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/mesh.h"

#pragma comment(lib, "Core/Assimp/libx86/assimp.lib")

ImportManager::ImportManager(Application* app, bool start_enabled) : Module(app, start_enabled) {


}


ImportManager::~ImportManager() {


}

bool ImportManager::Init() {

	MeshImporter::Init();
	TextureImporter::Init();

	return true;
}

bool ImportManager::CleanUp() {

	MeshImporter::CleanUp();
	TextureImporter::CleanUp();

	return true;
}

PathInfo ImportManager::GetPathInfo(string path) {

	PathInfo pathInfo;
	App->file_system->SplitFilePath(path.c_str(), &pathInfo.path, &pathInfo.name, &pathInfo.format);
	return pathInfo;
}


bool ImportManager::LoadGeometry(const char* path) {

	//Root starting empty gO and scene info.
	GameObject* root = nullptr;


	/*
	if (buffer == nullptr) {
		string norm_path_short = "Assets/Models/" + App->file_system->SetNormalName(path);
		bytesFile = App->file_system->Load(norm_path_short.c_str(), &buffer);
	}
	*/



	if (scene != nullptr && scene->HasMeshes()) {

		aiNode* rootScene = scene->mRootNode;
		root = LoadNodes(scene, rootScene, buffer, path);

		aiReleaseImport(scene);
		RELEASE_ARRAY(buffer);
	}


	return true;
}

GameObject* ImportManager::LoadNodes(const aiScene* scene, aiNode* node, char* fileBuffer, const char* path) {

	//GO
	GameObject* new_go = nullptr;

	//Transform
	ComponentTransform* transform = nullptr;


	//Mesh
	Mesh* mesh = nullptr;
	aiMesh* new_mesh = nullptr;

	//Textures
	Texture* ourTexture = nullptr;
	aiMaterial* texture = nullptr;



	new_go = App->scene->CreateGameObject();
	string name(node->mName.C_Str());
	int index = name.find("RootNode");
	if (index != 0) {
		new_go->name = node->mName.C_Str();
	}
	else {
		new_go->name = App->file_system->SetNormalName(path);
		new_go->name.erase(new_go->name.find_last_of("."));
	}

	LOG("GameObject Name: %s", node->mName.C_Str());

	// === Getting aiNode transform info === //
	transform = dynamic_cast<ComponentTransform*>(new_go->GetTransform());


	//Retrieve mesh data for each node
	if (node != nullptr && node->mNumMeshes > 0) {

		//Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (size_t i = 0; i < node->mNumMeshes; ++i)
		{

			// === Mesh Importer === //
			mesh = new Mesh();
			new_mesh = scene->mMeshes[node->mMeshes[i]];

			MeshImporter::Import(new_mesh, mesh);

			char* meshBuffer = nullptr;
			uint64 size = MeshImporter::Save(mesh, &meshBuffer);
			App->file_system->Save(SetPathFormated(node->mName.C_Str(), ImportType::ImportMesh).c_str(), meshBuffer, size);




			

			if (scene->HasMaterials()) {
				texture = scene->mMaterials[new_mesh->mMaterialIndex];

				//If we have any texture we get the path
				if (texture != nullptr) {

					//Creating our container to fill with data later
					ourTexture = new Texture(0, 0, 0, 0, nullptr);

					//Get texture path info from node
					aiString texture_path;
					texture->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);
					string normalizedPath = "Assets/Textures/" + App->file_system->SetNormalName(texture_path.C_Str());

					// === Texture Importer ==== //
					TextureImporter::Import(normalizedPath.c_str());

					char* imageBuffer = nullptr;
					uint64 size = TextureImporter::Save(&imageBuffer);
					App->file_system->Save(SetPathFormated(App->importer->GetPathInfo(path).name.c_str(), ImportType::ImportTexture).c_str(), imageBuffer, size);
					RELEASE_ARRAY(imageBuffer); //--> Points in same direction that ILubyte data created before

					TextureImporter::Load(SetPathFormated(App->importer->GetPathInfo(path).name.c_str(), ImportType::ImportTexture).c_str(), ourTexture);

				}
			}
		}
	}

	//Iterates each child, stores its info into root child vector, and save parent info for each child
	if (node->mNumChildren > 0) {
		for (int i = 0; i < node->mNumChildren; ++i) {

			GameObject* child = LoadNodes(scene, node->mChildren[i], fileBuffer, path);
			child->parent = new_go;
			child->parentId = new_go->Getuid();
			new_go->children.push_back(child);
		}

	}
	return new_go;
}


void ImportManager::GetTransformInfo(aiNode* node) {

	aiVector3D translation, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	//Todo: Save Into JSON scale rot translate for each node and generate localMatrix based on this values
}