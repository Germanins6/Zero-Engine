#include "Application.h"
#include "ZeroImporter.h"

//-- Assimp
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/mesh.h"

#pragma comment(lib, "Core/Assimp/libx86/assimp.lib")

// -- DevIL Image Library
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

//-- Devil Libs loading
#pragma comment(lib, "Core/DevIL/libx86/DevIL.lib")
#pragma comment(lib, "Core/DevIL/libx86/ILU.lib")
#pragma comment(lib, "Core/DevIL/libx86/ILUT.lib")

// ==== MESH ==== //

void MeshImporter::Init() {

	//Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

}

void MeshImporter::CleanUp() {

	//-- Detach log stream
	aiDetachAllLogStreams();
}

void MeshImporter::Import(const aiMesh* aiMesh, ResourceMesh* ourMesh){

	//Checking how long takes to import normal fbx
	Timer importTime;
	importTime.Start();

	//Vertex
	ourMesh->num_vertex = aiMesh->mNumVertices;
	ourMesh->vertex = new float[ourMesh->num_vertex * 3];
	memcpy(ourMesh->vertex, aiMesh->mVertices, sizeof(float) * ourMesh->num_vertex * 3);
	LOG("New mesh with %d vertices", ourMesh->num_vertex);

	//Index
	if (aiMesh->HasFaces()) {
		ourMesh->num_index = aiMesh->mNumFaces * 3;
		ourMesh->index = new uint[ourMesh->num_index];

		for (size_t i = 0; i < aiMesh->mNumFaces; i++)
		{
			if (aiMesh->mFaces[i].mNumIndices != 3)
				LOG("WARNING, geometry face with != 3 indices!")
			else
				memcpy(&ourMesh->index[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
		}
	}

	//Normals
	if (aiMesh->HasNormals()) {

		//Initialize size
		ourMesh->num_normals = aiMesh->mNumVertices;
		ourMesh->normals = new float[aiMesh->mNumVertices * 3];

		//Calculate Normals of Vertex
		for (size_t i = 0; i < aiMesh->mNumVertices; i++) {

			//Calculate Normals of Vertex
			ourMesh->normals[i * 3] = aiMesh->mNormals[i].x;
			ourMesh->normals[i * 3 + 1] = aiMesh->mNormals[i].y;
			ourMesh->normals[i * 3 + 2] = aiMesh->mNormals[i].z;
		}
	}

	//Uvs
	ourMesh->num_uvs = aiMesh->mNumVertices;
	ourMesh->uv_coords = new float[ourMesh->num_vertex * 2];

	for (size_t i = 0; i < aiMesh->mNumVertices; i++) {

		if (aiMesh->mTextureCoords[0]) {
			ourMesh->uv_coords[i * 2] = aiMesh->mTextureCoords[0][i].x;
			ourMesh->uv_coords[i * 2 + 1] = aiMesh->mTextureCoords[0][i].y;
		}
	}

	LOG("FBX took %d ms to be imported", importTime.Read());
}

uint64 MeshImporter::Save(const ResourceMesh* ourMesh, char** fileBuffer) {

	uint ranges[4] = { ourMesh->num_index, ourMesh->num_vertex, ourMesh->num_normals, ourMesh->num_uvs};

	uint size = sizeof(ranges) + sizeof(uint) * ourMesh->num_index + sizeof(float) * ourMesh->num_vertex * 3 + sizeof(float) * ourMesh->num_normals * 3 + sizeof(float) * ourMesh->num_uvs * 2;

	char* BufferSize = new char[size];
	char* cursor = BufferSize;

	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	//Index
	bytes = sizeof(uint) * ourMesh->num_index;
	memcpy(cursor, ourMesh->index, bytes);
	cursor += bytes;

	//Vertex
	bytes = sizeof(float) * ourMesh->num_vertex * 3;
	memcpy(cursor, ourMesh->vertex, bytes);
	cursor += bytes;

	//Normals
	bytes = sizeof(float) * ourMesh->num_normals * 3;
	memcpy(cursor, ourMesh->normals, bytes);
	cursor += bytes;

	//Uvs
	bytes = sizeof(float) * ourMesh->num_uvs * 2;
	memcpy(cursor, ourMesh->uv_coords, bytes);
	cursor += bytes;

	*fileBuffer = BufferSize;

	return size;
}

void MeshImporter::Load(const char* fileBuffer, ResourceMesh* ourMesh) {

	//Checking how long takes to load our own mesh structure
	Timer loadTime;
	loadTime.Start();

	//Creating buffer to store fileBuffer info into to be used later by our cursor to load mesh info
	char* buffer;
	App->file_system->Load(fileBuffer, &buffer);
	char* cursor = buffer;

	//Mirror steps as Save
	uint ranges[4];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	ourMesh->num_index = ranges[0];
	ourMesh->num_vertex = ranges[1];
	ourMesh->num_normals = ranges[2];
	ourMesh->num_uvs = ranges[3];

	//Index
	bytes = sizeof(uint) * ourMesh->num_index;
	ourMesh->index = new uint[ourMesh->num_index];
	memcpy(ourMesh->index, cursor, bytes);
	cursor += bytes;

	//Vertex
	bytes = sizeof(float) * ourMesh->num_vertex * 3;
	ourMesh->vertex = new float[ourMesh->num_vertex * 3];
	memcpy(ourMesh->vertex, cursor, bytes);
	cursor += bytes;

	//Normals
	bytes = sizeof(float) * ourMesh->num_normals * 3;
	ourMesh->normals = new float[ourMesh->num_normals * 3];
	memcpy(ourMesh->normals, cursor, bytes);
	cursor += bytes;

	//Uvs
	bytes = sizeof(float) * ourMesh->num_uvs * 2;
	ourMesh->uv_coords = new float[ourMesh->num_uvs * 2];
	memcpy(ourMesh->uv_coords, cursor, bytes);
	cursor += bytes;

	//meshes.push_back(ourMesh);

	LOG("Own mesh took %d ms to be loaded", loadTime.Read());
}


// ==== TEXTURE ==== //


void TextureImporter::Init() {

	//Devil Ilu and Il initialization
	ilInit();
	iluInit();
	ilutInit();

	if (ilutRenderer(ILUT_OPENGL))
		LOG("DevIL succesfully loaded: Renderer set as OpenGL | Libs initialized")
	else 
		LOG("Error trying load DevIL renderer");

}

void TextureImporter::CleanUp() {

}

void TextureImporter::Import(const char* path) {

	Timer imageImport;
	imageImport.Start();

	char* buffer = nullptr;
	uint size = App->file_system->Load(path, &buffer);

	if (buffer != nullptr) {
		if (ilLoadL(IL_TYPE_UNKNOWN, buffer, size) == IL_FALSE) {
			if (ilLoadImage(path) == IL_FALSE)
				LOG("Source image from %s path Imported Succesfully", path)
			else
				LOG("Unable to import texture");
		}
	}

	LOG("Image file took %d ms to be imported", imageImport.Read());
}

uint64 TextureImporter::Save(char** fileBuffer) {

	ILuint size;

	ILubyte* data;


	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, nullptr, 0);

	if (size > 0) {

		data = new ILubyte[size];
		if (ilSaveL(IL_DDS, data, size) > 0)
			*fileBuffer = (char*)data;
	}

	return size;
}

void TextureImporter::Load(const char* fileBuffer, ResourceTexture* ourTexture, TextureSettings importSettings) {

	Timer imageLoad;
	imageLoad.Start();


	if (ourTexture->data != nullptr)
		glDeleteTextures(1, &ourTexture->gpu_id);

	ILuint temp = 0;
	ilGenImages(1, &temp);
	ilBindImage(temp);

	ilEnable(IL_ORIGIN_SET);
	
	//Flip Import options
	switch (importSettings.flipMode) {
	case flipMode::FlipNone: ilOriginFunc(IL_ORIGIN_LOWER_LEFT); break;
	case flipMode::FlipX: break;
	case flipMode::FlipY: ilOriginFunc(IL_ORIGIN_UPPER_LEFT); break;
	case flipMode::FlipXY: 	iluFlipImage(); break;
	}

	char* buffer;
	uint size = App->file_system->Load(fileBuffer, &buffer);
	
	if (ilLoadL(IL_DDS, buffer, size) == IL_FALSE)
		LOG("Unable to load texture");

	//Initialitizing texture values and buff
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	//Store texture info into ResourceTexture values
	ourTexture->gpu_id = temp;
	ourTexture->height = ilGetInteger(IL_IMAGE_HEIGHT);
	ourTexture->width = ilGetInteger(IL_IMAGE_WIDTH);
	ourTexture->type = ilGetInteger(IL_IMAGE_FORMAT);
	ourTexture->data = ilGetData();

	//Create textures (diffuse,normal,specular...) into glBuffers
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


	glGenTextures(1, (GLuint*)&(ourTexture->gpu_id));
	glBindTexture(GL_TEXTURE_2D, ourTexture->gpu_id);

	//Wrapping Importing options
	switch(importSettings.wrapMode){
	case WrappingMode::Clamp: 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case WrappingMode::Repeat : 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case WrappingMode::ClampBorder:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	case WrappingMode::MirroredRepeat:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}
	
	//MipMap Importing options
	if (importSettings.enableMipMap) {

		//glGenerateMipmap(GL_TEXTURE_2D);

		if (importSettings.filterMode == filteringMode::FilterNearest) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		}
		
		if (importSettings.filterMode == filteringMode::FilterLinear) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		}

	}
	else {

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	//Generate Image with info
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)ourTexture->GetWidth(), (int)ourTexture->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)ourTexture->data);
	
	//Unbind IL and GL
	glBindTexture(GL_TEXTURE_2D, 0);
	ilBindImage(0);

	LOG("Succesfully image loaded with: ID %u SIZE %u X %u", ourTexture->gpu_id, ourTexture->width, ourTexture->height);
	LOG("Own Image file took %d ms to be loaded", imageLoad.Read());
}

// ==== MODEL ==== //

void ModelImporter::Import(const char* path, ResourceModel* ourModel) {

	Timer modelImport;
	modelImport.Start();

	const aiScene* scene = nullptr;

	char* buffer;
	string resourcePath = App->file_system->NormalizePath(path);
	resourcePath = App->file_system->SetNormalName(resourcePath.c_str());
	resourcePath = "Assets/Models/" + resourcePath;

	uint bytesFile = App->file_system->Load(resourcePath.c_str(), &buffer);

	//Checks if buffer its empty or not and load file from that resource, if not we load from path
	if (buffer != nullptr)
		scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, NULL);
	else
		scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
		

	if (scene != nullptr && scene->HasMeshes()) {

		aiNode* node = scene->mRootNode;

		//Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{ 
			//Retrieve mesh data for each node
			aiMesh* aiMesh = scene->mMeshes[i];
			ResourceMesh* mesh = (ResourceMesh*)App->resources->ImportAssimpStuff(resourcePath.c_str(), ResourceType::Mesh, aiMesh);
			ourModel->meshes.push_back(mesh);

			App->resources->SaveResource(mesh);
		}

		//Use scene->mNumMaterials to iterate on scene->mMaterials array
		for (size_t i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* aiMaterial = scene->mMaterials[i];
			ResourceMaterial* material = (ResourceMaterial*)App->resources->ImportAssimpStuff(resourcePath.c_str(), ResourceType::Material, nullptr, aiMaterial);
			ourModel->materials.push_back(material);

			App->resources->SaveResource(material);
		}

		//Recursive function that will retrieve each node info stored into
		ModelImporter::ImportNodes(scene, node, ourModel);
		
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", path);


	RELEASE_ARRAY(buffer);

	LOG("Model took %d ms to be imported", modelImport.Read());
}

int ModelImporter::ImportNodes(const aiScene* scene, aiNode* node, ResourceModel* ourModel, int iterator, UID parentId) {

	//Bring ImportManager.cpp -- > LoadNodes here
	Model.AddUnsignedInt("-Num_Children", iterator);

	Model.Object[to_string(iterator)];
	
	string name(node->mName.C_Str());

	if (name == "RootNode")
		Model.AddStringObj("Name", App->resources->GetPathInfo(ourModel->assetsFile).name, to_string(iterator));
	else
		Model.AddStringObj("Name", node->mName.C_Str(), to_string(iterator));

	UID rootUID = App->resources->GenerateNewUID();
	Model.AddUnsignedIntObj("ID", rootUID, to_string(iterator));
	Model.AddUnsignedIntObj("IDParent", parentId, to_string(iterator));
	ImportTransformInfo(node, iterator);

	//If actual node have a mesh we store uid value into json to be loaded later from our resource manager in Model::Load
	if (node->mMeshes != nullptr) {
		UID meshID = stoi(App->resources->GetPathInfo(ourModel->meshes[*node->mMeshes]->GetLibraryFile()).name);
		Model.AddUnsignedIntObj("MeshUID", meshID, to_string(iterator));
	}
	else{ 
		Model.AddUnsignedIntObj("MeshUID", 0 , to_string(iterator));
	}

	//Store material uids into respective gameObject using it
	if (node->mMeshes != nullptr) {
		UID materialID = stoi(App->resources->GetPathInfo(ourModel->materials[scene->mMeshes[*node->mMeshes]->mMaterialIndex]->GetLibraryFile()).name);
		Model.AddUnsignedIntObj("MaterialUID", materialID , to_string(iterator));
	}
	else
		Model.AddUnsignedIntObj("MaterialUID", 0 , to_string(iterator));

	Model.AddBoolObj("HasCamera", false, to_string(iterator));
	Model.AddBoolObj("HasRigidbody", false, to_string(iterator));
	Model.AddBoolObj("HasCollider", false, to_string(iterator));
	Model.AddBoolObj("HasMaterial", false, to_string(iterator));
	Model.AddBoolObj("HasDistanceJoint", false, to_string(iterator));


	//Iterates each child, stores its info into root child vector, and save parent info for each child recursively
	if (node->mNumChildren > 0)
		for (int i = 0; i < node->mNumChildren; ++i)
			iterator = ImportNodes(scene, node->mChildren[i], ourModel, ++iterator, rootUID);

	return iterator;
}

void ModelImporter::ImportTransformInfo(aiNode* node, int iterator) {

	aiVector3D translation, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	Model.AddFloat3Obj("Translation", { translation.x, translation.y , translation.z }, to_string(iterator));
	Model.AddQuaternionObj("Rotation", { rotation.x, rotation.y, rotation.z , rotation.w}, to_string(iterator));
	Model.AddFloat3Obj("Scale", { scaling.x, scaling.y, scaling.z }, to_string(iterator));
}

uint64 ModelImporter::Save(const ResourceModel* ourModel) {

	Model.Save(ourModel->libraryFile.c_str());
	return -1;
}

void ModelImporter::Load(const char* fileBuffer) {

	//Open Model file from Lib
	Model.Load(fileBuffer);
	
	//By default constructor creates transform
	for (size_t i = 0; i <= Model.GetUnsignedInt("-Num_Children"); i++)
	{
		GameObject* gameObject = new GameObject();

		gameObject->name = Model.GetStringObj("Name", to_string(i));
		gameObject->uuid = Model.GetUnsignedIntObj("ID", to_string(i));
		gameObject->parentId = Model.GetUnsignedIntObj("IDParent", to_string(i));

		//Parent child to parent and parent to child
		if (gameObject->parentId != 0) {
			gameObject->parent = App->resources->SearchGameObjectByUID(gameObject->parentId);
			App->resources->SearchGameObjectByUID(gameObject->parentId)->children.push_back(gameObject);
		}

		//Transform
		float3 translate = Model.GetFloatXYZObj("Translation", to_string(i));
		Quat rotation = Model.GetQuaternionObj("Rotation", to_string(i));
		float3 scale = Model.GetFloatXYZObj("Scale", to_string(i));

		ComponentTransform* transform = dynamic_cast<ComponentTransform*>(gameObject->GetTransform());
		transform->SetPosition(translate);
		transform->euler = rotation.ToEulerXYZ() * RADTODEG;
		transform->SetRotation(transform->euler);
		transform->SetScale(scale);
		transform->UpdateNodeTransforms();

		//Mesh info -> Check First if resource exist ? Request -> If request fails load and request again
		UID meshUID = Model.GetUnsignedIntObj("MeshUID", to_string(i));
		if (meshUID != 0)
			if (App->resources->RequestResource(meshUID, true) == nullptr) {
				App->resources->LoadResource("Asset", ResourceType::Mesh, meshUID);
				gameObject->CreateComponent(ComponentType::MESH, App->resources->RequestResource(meshUID));
			}
			else {
				gameObject->CreateComponent(ComponentType::MESH, App->resources->RequestResource(meshUID));
			}
		
		//Material info -> Same process as mesh
		UID materialUID = Model.GetUnsignedIntObj("MaterialUID", to_string(i));
		if (materialUID != 0)
			if (App->resources->RequestResource(materialUID, true) == nullptr) {
				App->resources->LoadResource("Asset", ResourceType::Material, materialUID);
				gameObject->CreateComponent(ComponentType::MATERIAL, App->resources->RequestResource(materialUID));
			}
			else {
				gameObject->CreateComponent(ComponentType::MATERIAL, App->resources->RequestResource(materialUID));
			}

		//Camera info
		bool HasCamera = Model.GetBoolObj("HasCamera", to_string(i));
		if (HasCamera) {
			gameObject->CreateComponent(ComponentType::CAMERA);
			ComponentCamera* camera = gameObject->GetCamera();
			camera->SetNearDistance(Model.GetFloatObj("Near Distance",to_string(i)));
			camera->SetFarDistance(Model.GetFloatObj("Far Distance", to_string(i)));
			camera->SetFOV(Model.GetFloatObj("Field of View", to_string(i)));
		}

		//Rigidbody
		bool HasRigidbody = Model.GetBoolObj("HasRigidbody", to_string(i));
		if (HasRigidbody) {
			gameObject->CreateComponent(ComponentType::RIGIDBODY);
			ComponentRigidDynamic* rigidbody = gameObject->GetRigidbody();
			
			bool use_gravity = Model.GetBoolObj("EnableGravity", to_string(i));
			rigidbody->EnableGravity(use_gravity);

			bool use_kinematic = Model.GetBoolObj("isKinematic", to_string(i));
			rigidbody->EnableKinematic(use_kinematic);

			float mass = Model.GetFloatObj("Mass", to_string(i));
			rigidbody->SetMass(mass);

			float density = Model.GetFloatObj("Density", to_string(i));
			rigidbody->SetDensity(density);

			float linear_damping = Model.GetFloatObj("Linear Damping", to_string(i));
			rigidbody->SetLinearDamping(linear_damping);

			float angular_damping = Model.GetFloatObj("Angular Damping", to_string(i));
			rigidbody->SetAngularDamping(angular_damping);

			float3 force = Model.GetFloatXYZObj("Force", to_string(i));
			rigidbody->AddForce(force);

			float3 torque = Model.GetFloatXYZObj("Torque", to_string(i));
			rigidbody->AddTorque(torque);

			float3 linear_vel = Model.GetFloatXYZObj("Linear Velocity", to_string(i));
			rigidbody->SetLinearVelocity(linear_vel);

			float3 angular_vel = Model.GetFloatXYZObj("Angular Velocity", to_string(i));
			rigidbody->SetAngularVelocity(angular_vel);

			bool lockLinearX = Model.GetBoolObj("LockLinearX", to_string(i));
			bool lockLinearY = Model.GetBoolObj("LockLinearY", to_string(i));
			bool lockLinearZ = Model.GetBoolObj("LockLinearZ", to_string(i));
			rigidbody->LockLinearX(lockLinearX);
			rigidbody->LockLinearY(lockLinearY);
			rigidbody->LockLinearZ(lockLinearZ);

			bool lockAngularX = Model.GetBoolObj("LockAngularX", to_string(i));
			bool lockAngularY = Model.GetBoolObj("LockAngularY", to_string(i));
			bool lockAngularZ = Model.GetBoolObj("LockAngularZ", to_string(i));
			rigidbody->LockAngularX(lockAngularX);
			rigidbody->LockAngularY(lockAngularY);
			rigidbody->LockAngularZ(lockAngularZ);
		}

		//Collider
		bool HasCollider = Model.GetBoolObj("HasCollider", to_string(i));
		if (HasCollider) {
			gameObject->CreateComponent(ComponentType::COLLIDER);
			ComponentCollider* collider = gameObject->GetCollider();

			collider->isTrigger = Model.GetBoolObj("isTrigger", to_string(i));

			bool HasMaterial = Model.GetBoolObj("HasMaterial", to_string(i));
			if (HasMaterial) {
				float staticFriction = Model.GetFloatObj("StaticFriction", to_string(i));
				float dynamicFriction = Model.GetFloatObj("DynamicFriction", to_string(i));
				float restitution = Model.GetFloatObj("Restitution", to_string(i));
				collider->colliderMaterial = App->physX->mPhysics->createMaterial(staticFriction, dynamicFriction, restitution);
			}

			collider->SetPosition(Model.GetFloatXYZObj("Center", to_string(i)));
			collider->SetRotation(Model.GetFloatXYZObj("Euler", to_string(i)));
			collider->colliderRot = Model.GetQuaternionObj("Quat" , to_string(i));
			collider->SetScale(Model.GetFloatXYZObj("Size", to_string(i)));
		}

		//DistanceJoint
		bool HasDistanceJoint = Model.GetBoolObj("HasDistanceJoint", to_string(i));
		if (HasDistanceJoint) {

		}

		//Store gameObject into scene vector
		App->scene->gameobjects.push_back(gameObject);
	}
}

// ==== MATERIAL ==== //

void MaterialImporter::Import(const aiMaterial* aiMaterial, ResourceMaterial* ourMaterial) {
	Timer materialImport;
	materialImport.Start();

	if (aiMaterial != nullptr) {

		//Get texture path info from node
		aiString texture_path;
		aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);

		aiColor4D diffuseRGBA;
		aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseRGBA);

		string normalizedPath = texture_path.C_Str();
		normalizedPath = normalizedPath.substr(normalizedPath.find_last_of(0x5c) + 1);

		//Store always color diffuse info independently if any mesh does have texture or not.
		ourMaterial->materialColor.r = diffuseRGBA.r;
		ourMaterial->materialColor.g = diffuseRGBA.g;
		ourMaterial->materialColor.b = diffuseRGBA.b;
		ourMaterial->materialColor.a = diffuseRGBA.a;

		//Store and import image files if any material use it as diffuse.
		if (normalizedPath.size() > 0) {
			string real_path = "Assets/Textures/";
			string str_texture(normalizedPath.c_str());
			real_path += str_texture;

			//If the current file actually exists doesnt need to be imported once again
			if (!App->resources->CheckMetaFileExists(real_path.c_str())) {
				ourMaterial->diffuse_id = App->resources->ImportFile(real_path.c_str());
			}
			else {
				TextureImporter::Texture.Load(real_path.append(".meta").c_str());
				string LibPath = TextureImporter::Texture.GetString("LibraryPath");
				ourMaterial->diffuse_id = stoi(App->resources->GetPathInfo(LibPath).name);
			}
		}
	}

	LOG("Material took %d ms to be imported", materialImport.Read());
}

uint64 MaterialImporter::Save(ResourceMaterial* ourMaterial) {

	//Add info into json file and save in Library
	Material.AddUnsignedInt("Diffuse", ourMaterial->diffuse_id);
	Material.AddString("Diffuse_libraryPath", App->resources->SetPathFormated(ourMaterial->diffuse_id, ResourceType::Texture));
	Material.AddString("Diffuse_assetPath", App->resources->SetPathFormated(ourMaterial->diffuse_id, ResourceType::Texture));
	Material.AddFloat("R", ourMaterial->materialColor.r);
	Material.AddFloat("G", ourMaterial->materialColor.g);
	Material.AddFloat("B", ourMaterial->materialColor.b);
	Material.AddFloat("A", ourMaterial->materialColor.a);

	Material.Save(ourMaterial->libraryFile.c_str());
	return -1;
}

void MaterialImporter::Load(const char* fileBuffer, ResourceMaterial* ourMaterial) {
	Timer materialLoad;
	materialLoad.Start();

	//Open file with json and retrieves uid from Diffuse channel
	Material.Load(fileBuffer);

	ourMaterial->materialColor.r = Material.GetFloat("R");
	ourMaterial->materialColor.g = Material.GetFloat("G");
	ourMaterial->materialColor.b = Material.GetFloat("B");
	ourMaterial->materialColor.a = Material.GetFloat("A");

	UID textureUID = Material.GetUnsignedInt("Diffuse");
	string libPath = Material.GetString("Diffuse_libraryPath");
	string assetPath = Material.GetString("Diffuse_assetPath");

	//Get resource, either if loaded or not for verification purpose, and in case of exist doesnt increment referenceCount
	Resource* resourceTexture = App->resources->RequestResource(textureUID);

	//At same time we load a material we should be loading texture and create if still doesnt exist as resource
	if (textureUID != 0 && resourceTexture != nullptr) {
		ourMaterial->diffuse = dynamic_cast<ResourceTexture*>(resourceTexture);
	}
	else if(textureUID != 0 && resourceTexture == nullptr){
		resourceTexture = App->resources->CreateNewResource( fileBuffer, ResourceType::Texture, true, textureUID);
		TextureImporter::Load(libPath.c_str(), dynamic_cast<ResourceTexture*>(resourceTexture), App->editor->textureSettings);
		ourMaterial->diffuse = dynamic_cast<ResourceTexture*>(App->resources->RequestResource(textureUID));
	}

	LOG("Own Material took %d ms to be loaded", materialLoad.Read());
}