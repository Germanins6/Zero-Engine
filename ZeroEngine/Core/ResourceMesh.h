#include "Resource.h"


class ResourceMesh : public Resource {

public:
	ResourceMesh(UID id);
	~ResourceMesh();

	void Save() const;
	void Load();
	bool LoadInMemory();

public:

};