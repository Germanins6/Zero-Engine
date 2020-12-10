#pragma once

#include "Resource.h"
#include "ResourceTexture.h"
#include "Color.h"

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

	Color materialColor;

	/*
	ResourceTexture* specular;
	ResourceTexture* normal;
	ResourceTexture* specular;
	*/
};