#pragma once

//-- Tools 
#include "p2Defs.h"
#include <vector>
#include "SDL/include/SDL_opengl.h"
#include "JSON/json.hpp"

//-- Assimp
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/mesh.h"

#pragma comment(lib, "Core/Assimp/libx86/assimp.lib")

// -- DevIL Image Library
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#include "glew/include/glew.h"

//-- Devil Libs loading
#pragma comment(lib, "Core/DevIL/libx86/DevIL.lib")
#pragma comment(lib, "Core/DevIL/libx86/ILU.lib")
#pragma comment(lib, "Core/DevIL/libx86/ILUT.lib")

//-- Namespaces
using namespace std;

// --Forward declarations
class Mesh;
struct Texture;
class ResourceMesh;
class ResourceTexture;
class ResourceModel;

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
	uint64 Save(const ResourceModel* ourModel, char** fileBuffer);
	GameObject* Load(const char* fileBuffer, ResourceModel* ourModel);
}