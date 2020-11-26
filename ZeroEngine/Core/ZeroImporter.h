#pragma once

//-- Tools 
#include "p2Defs.h"
#include <vector>
#include "Serialization.h"
#include "GameObject.h"

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ResourceModel.h"
#include "ResourceMaterial.h"

//-- Namespaces
using namespace std;

// --Forward declarations
class Mesh;
struct Texture;

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

namespace MeshImporter {

	void Init();
	void CleanUp();

	void Import(const aiMesh* aiMesh, ResourceMesh* ourMesh);
	uint64 Save(const ResourceMesh* ourMesh, char** fileBuffer);
	void Load(const char* fileBuffer, Mesh* ourMesh);

}

namespace TextureImporter {

	void Init();
	void CleanUp();

	void Import(const char* path);
	uint64 Save(char** fileBuffer);
	void Load(const char* fileBuffer, Texture* ourTexture);

}

namespace ModelImporter {

	void Import(const char* path, ResourceModel* ourModel);
	void ImportNodes(const aiScene* scene, aiNode* node, char* fileBuffer, const char* path);
	void ImportTransformInfo(aiNode* node);
	uint64 Save(const ResourceModel* ourModel);
	GameObject* Load(const char* fileBuffer, ResourceModel* ourModel);
}

namespace MaterialImporter {

	void Import(const aiMaterial* aiMaterial, ResourceMaterial* ourMaterial);
	uint64 Save(ResourceMaterial* ourMaterial, char* fileBuffer);
	void Load(const char* fileBuffer, ResourceMaterial* ourMaterial);
}