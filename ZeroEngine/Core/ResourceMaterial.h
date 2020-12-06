#pragma once

#include "Resource.h"
#include "ResourceTexture.h"

class ResourceMaterial : public Resource {

public:
	ResourceMaterial(UID uid);
	~ResourceMaterial();

	void Save() const;
	void Load();
	bool LoadInMemory();

public: 

	//Channels
	ResourceTexture* diffuse;
	UID diffuse_id;

	/*
	ResourceTexture* specular;
	ResourceTexture* normal;
	ResourceTexture* specular;
	*/
};