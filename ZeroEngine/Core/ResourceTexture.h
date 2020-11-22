#pragma once

#include "Resource.h"

class ResourceTexture : public Resource {

public:
	ResourceTexture(UID id);
	~ResourceTexture();
	void Save() const ;
	void Load();
	bool LoadInMemory();

	inline UID GetTextureID() const { return gpu_id; };
	inline uint GetWidth() const { return width; };
	inline uint GetHeight() const { return height; };

public:

	uint gpu_id;
	uint height;
	uint width; 
	uint type;

};