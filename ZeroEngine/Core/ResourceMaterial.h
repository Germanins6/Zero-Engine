#pragma once

#include "Resource.h"

class ResourceMaterial : public Resource {

public:
	ResourceMaterial(UID uid);
	~ResourceMaterial();

	void Save() const;
	void Load();
	bool LoadInMemory();

public: 

	UID diffuse;
};