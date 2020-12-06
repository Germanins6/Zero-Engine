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
	UID diffuse;

	/*
	UID specular;
	UID normal;
	UID specular;
	*/
};