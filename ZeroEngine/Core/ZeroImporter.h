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
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

enum WrappingMode {
	Repeat,
	MirroredRepeat,
	Clamp,
	ClampBorder,
};

enum flipMode{
	FlipNone,
	FlipX,
	FlipY,
	FlipXY,
};

enum filteringMode {
	FilterNone,
	FilterMipMapNearest,
	FilterMipMapLinear,
	FilterNearest,
	FilterLinear,
};

struct ModelSettings {

	ModelSettings(int scale = 1, bool camera = true, bool light = true) : globalScale{ scale },cameraImport{ camera }, lightImport{ light }{}

	int globalScale;
	bool cameraImport;
	bool lightImport;
};

struct TextureSettings {

	TextureSettings(bool mipmap = false, filteringMode filter = filteringMode::FilterNone, flipMode flip = flipMode::FlipNone, WrappingMode wrap = WrappingMode::Repeat) : enableMipMap{mipmap}, filterMode { filter}, flipMode{flip}, wrapMode{wrap}{}

	bool enableMipMap;
	filteringMode filterMode;
	flipMode flipMode;
	WrappingMode wrapMode;
};

namespace MeshImporter {

	void Init();
	void CleanUp();

	void Import(const aiMesh* aiMesh, ResourceMesh* ourMesh);
	uint64 Save(const ResourceMesh* ourMesh, char** fileBuffer);
	void Load(const char* fileBuffer, ResourceMesh* ourMesh);

}

namespace TextureImporter {

	void Init();
	void CleanUp();

	void Import(const char* path);
	uint64 Save(char** fileBuffer);
	void Load(const char* fileBuffer, ResourceTexture* ourTexture, TextureSettings importSettings);

	static Serializer Texture;
}

namespace ModelImporter {

	void Import(const char* path, ResourceModel* ourModel);
	int ImportNodes(const aiScene* scene, aiNode* node, ResourceModel* ourModel, int iterator = 0, UID id = 0);
	void ImportTransformInfo(aiNode* node, int iterator);
	uint64 Save(const ResourceModel* ourModel);
	void Load(const char* fileBuffer);

	static Serializer Model;
}

namespace MaterialImporter {

	void Import(const aiMaterial* aiMaterial, ResourceMaterial* ourMaterial);
	uint64 Save(ResourceMaterial* ourMaterial);
	void Load(const char* fileBuffer,ResourceMaterial* ourMaterial);
	
	static Serializer Material;
}