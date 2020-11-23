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

using namespace std;

namespace MeshImporter {

	void Init();
	void CleanUp();

	void Import(const aiMesh* aiMesh, Mesh* ourMesh);
	uint64 Save(const Mesh* ourMesh, char** fileBuffer);
	void Load(const char* fileBuffer, Mesh* ourMesh);

	//vector <Mesh*> meshes;

}

namespace TextureImporter {

	void Init();
	void CleanUp();

	void Import(const char* path);
	uint64 Save(char** fileBuffer);
	void Load(const char* fileBuffer, Texture* ourTexture);

	//vector<Texture*> textures;
}

namespace ModelImporter {

	void Import(const char* path, ResourceModel* ourModel);
	uint64 Save(const ResourceModel* ourModel, char** fileBuffer);
}