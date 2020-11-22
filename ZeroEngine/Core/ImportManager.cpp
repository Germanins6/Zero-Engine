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

bool ImportManager::LoadGeometry(const char* path) {

	//Root starting empty gO and scene info.
	GameObject* root = nullptr;
	const aiScene* scene = nullptr;

	//Create path buffer and import to scene
	char* buffer = nullptr;
	uint bytesFile = App->file_system->Load(path, &buffer);


	if (buffer == nullptr) {
		string norm_path_short = "Assets/Models/" + App->file_system->SetNormalName(path);
		bytesFile = App->file_system->Load(norm_path_short.c_str(), &buffer);
	}

	if (buffer != nullptr) scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, NULL);
	else scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);


	if (scene != nullptr && scene->HasMeshes()) {

		aiNode* rootScene = scene->mRootNode;
		root = LoadNodes(scene, rootScene, buffer, path);

		aiReleaseImport(scene);
		RELEASE_ARRAY(buffer);
	}
	else
		LOG("Error loading scene %s", path);


	RELEASE_ARRAY(buffer);

	return true;
}

void ImportManager::LoadTexture(const char* path) {
	TextureImporter::Import(path);
	char* buffer = nullptr;
	uint size = TextureImporter::Save(&buffer);
	App->file_system->Save(SetPathFormated(App->resources->GetPathInfo(path).name.c_str(), ImportType::ImportTexture).c_str(), buffer, size);
	RELEASE_ARRAY(buffer); //--> Points in same direction that ILubyte data created before
}

GameObject* ImportManager::LoadNodes(const aiScene* scene, aiNode* node, char* fileBuffer, const char* path) {

	//GO
	GameObject* new_go = nullptr;

	//Transform
	ComponentTransform* transform = nullptr;
	aiVector3D translation, scaling;
	aiQuaternion rotation;

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

	//Convert aiMatrix4x4 to float4x4 matrix and store values into transform
	float4x4 localMatrix;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			transform->localMatrix[i][j] = node->mTransformation[i][j];

	transform->UpdateLocalMatrix();

	node->mTransformation.Decompose(scaling, rotation, translation);

	Quat rot1 = { rotation.x, rotation.y, rotation.z , rotation.w };
	transform->euler = rot1.ToEulerXYZ() * RADTODEG;
	transform->SetPosition(translation.x, translation.y, translation.z);
	transform->SetRotation(transform->euler.x, transform->euler.y, transform->euler.z);
	transform->SetScale(scaling.x, scaling.y, scaling.z);

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


			MeshImporter::Load(SetPathFormated(node->mName.C_Str(), ImportType::ImportMesh).c_str(), mesh);

			new_go->CreateComponent(ComponentType::MESH, path, mesh);

			// === Node Transformation Info === //
			node->mTransformation.Decompose(scaling, rotation, translation);
			Quat rot2 = { rotation.x, rotation.y, rotation.z , rotation.w };
			transform->euler = rot2.ToEulerXYZ() * RADTODEG;
			transform->SetPosition(translation.x, translation.y, translation.z);
			transform->SetRotation(transform->euler.x, transform->euler.y, transform->euler.z);
			transform->SetScale(scaling.x, scaling.y, scaling.z);

			transform->UpdateGlobalMatrix();

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
					App->file_system->Save(SetPathFormated(App->resources->GetPathInfo(path).name.c_str(), ImportType::ImportTexture).c_str(), imageBuffer, size);
					RELEASE_ARRAY(imageBuffer); //--> Points in same direction that ILubyte data created before

					TextureImporter::Load(SetPathFormated(App->resources->GetPathInfo(path).name.c_str(), ImportType::ImportTexture).c_str(), ourTexture);

					//Setting texture info to componentMaterial
					new_go->CreateComponent(ComponentType::MATERIAL, normalizedPath.c_str(), nullptr, ourTexture);
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