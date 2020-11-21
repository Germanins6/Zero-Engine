#pragma once

#include "Resource.h"

class ResourceTexture : public Resource {

	ResourceTexture(uint id);
	virtual ~ResourceTexture();
	void Save() const override;
	void Load() override;

	inline uint GetTextureID() const { return gpu_id; };
	inline uint GetWidth() const { return width; };
	inline uint GetHeight() const { return height; };

	bool LoadInMemory() override;
	
public:

	uint gpu_id;
	uint height;
	uint width; 
	uint type;

};