


	


	/*
	if (buffer == nullptr) {
		string norm_path_short = "Assets/Models/" + App->file_system->SetNormalName(path);
		bytesFile = App->file_system->Load(norm_path_short.c_str(), &buffer);
	}
	



	if (scene != nullptr && scene->HasMeshes()) {

		aiNode* rootScene = scene->mRootNode;
		root = LoadNodes(scene, rootScene, buffer, path);

		aiReleaseImport(scene);
		RELEASE_ARRAY(buffer);
	}

	*/


/*
GameObject* ImportManager::LoadNodes(const aiScene* scene, aiNode* node, char* fileBuffer, const char* path) {

	//GO
	GameObject* new_go = nullptr;

	//Transform
	ComponentTransform* transform = nullptr;


	//Mesh
	Mesh* mesh = nullptr;
	aiMesh* new_mesh = nullptr;

	//Textures
	Texture* ourTexture = nullptr;
	aiMaterial* texture = nullptr;



	new_go = App->scene->CreateGameObject();
	string name(node->mName.C_Str());
	int index = name.find("RootNode");
	if (index != 0) {
		new_go->name = node->mName.C_Str();
	}
	else {
		new_go->name = App->file_system->SetNormalName(path);
		new_go->name.erase(new_go->name.find_last_of("."));
	}

	LOG("GameObject Name: %s", node->mName.C_Str());

	// === Getting aiNode transform info === //
	transform = dynamic_cast<ComponentTransform*>(new_go->GetTransform());


	//Retrieve mesh data for each node
	if (node != nullptr && node->mNumMeshes > 0) {

		
	}

	//Iterates each child, stores its info into root child vector, and save parent info for each child
	if (node->mNumChildren > 0) {
		for (int i = 0; i < node->mNumChildren; ++i) {

			GameObject* child = LoadNodes(scene, node->mChildren[i], fileBuffer, path);
			child->parent = new_go;
			child->parentId = new_go->Getuid();
			new_go->children.push_back(child);
		}

	}
	return new_go;
}

*/