#pragma once

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"

#include <vector>

using namespace std;

class ResourceModel : public Resource {

public:

	ResourceModel(UID id);
	~ResourceModel();
	bool LoadInMemory();

public:

	//Will store all meshes and textures contained when import assimp nodes
	vector<ResourceMesh*> meshes;
	vector<ResourceTexture*> textures;
};