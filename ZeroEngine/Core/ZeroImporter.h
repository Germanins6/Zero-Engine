#pragma once

#include "p2Defs.h"

class aiMesh;
class Mesh;

namespace MeshImporter {
	void Import(const aiMesh* aiMesh, Mesh* ourMesh);
	uint64 Save(const Mesh* ourMesh, char** fileBuffer);
	void Load(const char* fileBuffer, Mesh* ourMesh);
}

namespace TextureImporter {
}