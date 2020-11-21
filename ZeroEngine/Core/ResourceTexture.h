#pragma once

#include "Resource.h"

class ResourceTexture : public Resource {

	ResourceTexture(UID id);
	virtual ~ResourceTexture();
	void Save() const override;
	void Load() override;

	inline UID GetTextureID() const { return gpu_id; };
	inline uint GetWidth() const { return width; };
	inline uint GetHeight() const { return height; };

	bool LoadInMemory() override;
	
public:

	uint gpu_id;
	uint height;
	uint width; 
	uint type;

};