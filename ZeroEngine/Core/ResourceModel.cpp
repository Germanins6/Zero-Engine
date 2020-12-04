#include "ResourceModel.h"


ResourceModel::ResourceModel(UID uid) : Resource(uid, ResourceType::Model) {

}

ResourceModel::~ResourceModel() {

}

bool ResourceModel::LoadInMemory() {
	return 0;
}