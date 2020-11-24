#pragma once

#include "p2Defs.h"
#include <vector>
#include "JSON/json.hpp"

//Assimp forward declarations
class aiMesh;
class aiMaterial;

//Our data structures forward declaration
class Mesh;
struct Texture;
class ResourceMesh;
class ResourceTexture;
class ResourceModel;

using namespace std;

namespace MeshImporter {

	void Init();
	void CleanUp();

	void Import(const aiMesh* aiMesh, Mesh* ourMesh);
	uint64 Save(const Mesh* ourMesh, char** fileBuffer);
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
	uint64 Save(const ResourceModel* ourModel, char** fileBuffer);
	void Load(const char* fileBuffer, ResourceModel* ourModel);
}