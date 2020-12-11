#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"

//Empty GameObject constructor
GameObject::GameObject() {

	//Create name if empty gameobject, if not retrieves assimp node info
	name = name + ("EmptyObject_");
	parent = nullptr;

	name += std::to_string(App->scene->gameobjects.size());

	CreateComponent(ComponentType::TRANSFORM);


	active = true;
}

//GameObject creator when primitive created
/*
GameObject::GameObject(GameObject* owner, Mesh* data, PrimitiveTypesGL type) {

	data->type = type;
	parent = nullptr;

	//Set GameObject name depending path file info
	if (type == PrimitiveGL_Cube) { name = "Cube_"; name += std::to_string(App->scene->gameobjects.size()); }
	else if (type == PrimitiveGL_Cylinder) { name = "Cylinder_"; name += std::to_string(App->scene->gameobjects.size()); }
	else if (type == PrimitiveGL_Pyramid) { name = "Pyramid_"; name += std::to_string(App->scene->gameobjects.size()); }
	else if (type == PrimitiveGL_Sphere) { name = "Sphere_"; name += std::to_string(App->scene->gameobjects.size()); }

	//Creating always once transform component
	CreateComponent(ComponentType::TRANSFORM);

	//Create Directly Mesh Component
	CreateComponent(ComponentType::MESH, nullptr, data);

	active = true;
	draw_boundingBox = false;
}*/


GameObject::~GameObject() {

	// -- Cleaning components vector
	for (size_t i = 0; i < components.size(); i++) {
		RELEASE(components[i]);
	}

	components.clear();

	// -- Cleaning children vector
	/* for (size_t i = 0; i < children.size(); i++) {
		delete children[i];  ||||| children vector cant be already deleted once again
		because we call all gameObjects delete in Scene cleanUp , so all children will be erased in the process
		just fill with nullptr info and clean vector
		children[i] = nullptr;
	}
	

	children.clear();
	*/

	parent = nullptr;
}

void GameObject::Update(float dt) {
	UpdateBB();

	if (App->editor->draw_boundingBox == true || draw_boundingBox == true)
		DrawAABB();
}

//Create Component depending type received less mesh data that will 
Component* GameObject::CreateComponent(ComponentType type, Resource* ourResource) {

	Component* temp = nullptr;

	switch (type)
	{
	case ComponentType::TRANSFORM:
		temp = new ComponentTransform(this);
		break;
	case ComponentType::MATERIAL:
		temp = new ComponentMaterial(this, ourResource);
		break;
	case ComponentType::MESH:
		temp = new ComponentMesh(this, ourResource);
		break;
	case ComponentType::CAMERA:
		temp = new ComponentCamera(this);
		break;
	}

	this->components.push_back(temp);

	return temp;
}


//Search in the components vector the only transform available and returns info
Component* GameObject::GetTransform() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::TRANSFORM)
			return components[i];

	}

	return nullptr;
}

Component* GameObject::GetMesh() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::MESH)
			return components[i];

	}

	return nullptr;
}

Component* GameObject::GetMaterial() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::MATERIAL)
			return components[i];
	}

	return nullptr;
}

Component* GameObject::GetCamera() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::CAMERA)
			return components[i];

	}

	return nullptr;
}

string GameObject::SetName(string path) {

	int pos_igual = 0;

	string new_path = path;

	//Set the character we want to found
	char buscar = 0x5c;
	
	for (int i = 0; i <= new_path.size(); i++) {

		if (new_path[i] == buscar) {

			pos_igual = i + 1;
		}

	}

	return name.c_str();
}

void GameObject::ReParent(GameObject* child, GameObject* new_parent)
{
	GameObject* it = new_parent;

	//Erase the child of old parent
	if (child->parent != nullptr) {

		for (int i = 0; i < child->parent->children.size(); ++i)
			if (child->parent->children[i] == child)
			{
				child->parent->children.erase(child->parent->children.begin() + i);
				break;
			}
	}

	//Assign the new parent and pushback to children vector
	child->parent = new_parent;
	if (child->parent != nullptr)
		child->parent->children.push_back(child);

}


AABB GameObject::GetAABB() const { return bbox; }

void GameObject::UpdateBB() {

	math::OBB obb;
	obb.SetFrom(dynamic_cast<ComponentMesh*>(this->GetMesh())->GetAABB());
	obb.Transform(dynamic_cast<ComponentTransform*>(this->GetTransform())->GetGlobalMatrix().Transposed());
	bbox.SetNegativeInfinity();
	bbox.Enclose(obb);
}

void GameObject::DrawAABB() {

	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.25f, 1.0f, 0.0f, 1.0f);

	for (uint i = 0; i < bbox.NumEdges(); i++)
	{
		glVertex3f(bbox.Edge(i).a.x, bbox.Edge(i).a.y, bbox.Edge(i).a.z);
		glVertex3f(bbox.Edge(i).b.x, bbox.Edge(i).b.y, bbox.Edge(i).b.z);
	}
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	/*for (uint i = 0; i < owner->children.size(); ++i)
	{
		//dynamic_cast<ComponentMesh*>(owner->children[i]->GetMesh())->mesh->DrawAABB();
	}*/

}