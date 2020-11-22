#include "Resource.h"

Resource::Resource(UID id, ResourceType resourceType) {

	uid = id;
	type = resourceType;
	referenceCount = 0;

}

Resource::~Resource() {}

void Resource::Load() {

}

void Resource::Save() const {

}