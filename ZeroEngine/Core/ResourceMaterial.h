#pragma once

#include "Resource.h"

class ResourceMaterial : public Resource {

public:
	ResourceMaterial(UID uid);
	~ResourceMaterial();

public: 

	UID diffuse;
};