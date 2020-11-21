#include "Resource.h"

Resource::Resource(uint id, ResourceType resourceType) {

	uid = id;
	type = resourceType;
	referenceCount = 0;

}

Resource::~Resource() {}