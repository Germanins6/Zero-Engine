#pragma once

#include "p2Defs.h"
#include <vector>

class aiMesh;
class Mesh;
struct Texture;

namespace MeshImporter {
	void Import(const aiMesh* aiMesh, Mesh* ourMesh);
	uint64 Save(const Mesh* ourMesh, char** fileBuffer);
	void Load(const char* fileBuffer, Mesh* ourMesh);
}

namespace TextureImporter {

	bool Init();
	bool CleanUp();

	void Import(const char* pathFile, Texture* ourTexture);
	uint64 Save(Texture* ourTexture, char** fileBuffer);
	void Load(const char* fileBuffer, Texture* ourTexture);

	std::vector<Texture*> textures;
}