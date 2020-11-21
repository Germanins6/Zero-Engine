#pragma once

#include "p2Defs.h"
#include <string>

using namespace std;

enum class ResourceType {
	None = -1,
	Mesh,
	Texture,
	Max_Types,
};

class Resource {

public:

	//Constructor/Destructor and virtual calls
	Resource(uint uid, ResourceType type);
	virtual ~Resource();
	virtual void Save() const;
	virtual void Load();
	virtual bool LoadInMemory() = 0;

	//Getters
	inline ResourceType GetType() const { return type; };
	inline uint GetUID() const { return uid; };
	inline const char* GetAssetFile() const { return assetsFile.c_str(); };
	inline const char* GetLibraryFile() const { return libraryFile.c_str(); };
	inline uint GetReferenceCount() { return referenceCount; };

	bool IsLoaded() const;
	bool LoadToMemory();

protected:

	uint uid;
	string assetsFile;
	string libraryFile;

	ResourceType type;
	uint referenceCount;
};