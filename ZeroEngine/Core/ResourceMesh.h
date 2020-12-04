#pragma once

#include "Resource.h"


class ResourceMesh : public Resource {

public:
	ResourceMesh(UID id);
	~ResourceMesh();

	void Save() const;
	void Load();
	bool LoadInMemory();

public:

	uint num_index;
	uint* index;

	uint num_vertex;
	float* vertex;

	uint num_normals;
	float* normals;

	uint num_uvs;
	float* uv_coords;
};