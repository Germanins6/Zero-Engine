#pragma once

#include "DevIL/include/ilu.h"

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
	inline uint GetType() const { return type; };

public:

	ILuint gpu_id;
	ILint height, width, type;

	ILubyte* data;
};