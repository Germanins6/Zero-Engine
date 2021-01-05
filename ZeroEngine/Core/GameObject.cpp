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

GameObject::~GameObject() {

	// -- Cleaning components vector
	for (size_t i = 0; i < components.size(); i++)
		RELEASE(components[i]);
	
	components.clear();

	parent = nullptr;
}

void GameObject::Update(float dt) {

	UpdateBB();

	if (App->editor->draw_boundingBox == true || draw_boundingBox == true)
		DrawAABB();
}

//Create Component depending type received less mesh data that will 
Component* GameObject::CreateComponent(ComponentType type, Resource* ourResource, GeometryType geoType) {

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
	case ComponentType::RIGIDBODY:
		temp = new ComponentRigidDynamic(this);
		break;
	case ComponentType::COLLIDER:
		temp = new ComponentCollider(this, geoType);
		break;
	case ComponentType::DISTANCE_JOINT:
		temp = new ComponentDistanceJoint(this);
		break;
	case ComponentType::REVOLUTE_JOINT:
		temp = new ComponentRevoluteJoint(this);
		break;
	case ComponentType::PRISMATIC_JOINT:
		temp = new ComponentSliderJoint(this);
		break;
	case ComponentType::SPHERICAL_JOINT:
		temp = new ComponentSphericalJoint(this);
		break;
	}

	this->components.push_back(temp);

	return temp;
}


//Search in the components vector the only transform available and returns info
ComponentTransform* GameObject::GetTransform() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::TRANSFORM)
			return dynamic_cast<ComponentTransform*>(components[i]);
	}

	return nullptr;
}

ComponentMesh* GameObject::GetMesh() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::MESH)
			return dynamic_cast<ComponentMesh*>(components[i]);
	}

	return nullptr;
}

ComponentMaterial* GameObject::GetMaterial() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::MATERIAL)
			return dynamic_cast<ComponentMaterial*>(components[i]);
	}

	return nullptr;
}

ComponentCamera* GameObject::GetCamera() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::CAMERA)
			return dynamic_cast<ComponentCamera*>(components[i]);
	}

	return nullptr;
}

ComponentRigidDynamic* GameObject::GetRigidbody() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::RIGIDBODY)
			return dynamic_cast<ComponentRigidDynamic*>(components[i]);
	}

	return nullptr;
}

ComponentCollider* GameObject::GetCollider() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::COLLIDER)
			return dynamic_cast<ComponentCollider*>(components[i]);
	}

	return nullptr;
}

ComponentDistanceJoint* GameObject::GetDistanceJoint() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::DISTANCE_JOINT)
			return dynamic_cast<ComponentDistanceJoint*>(components[i]);
	}

	return nullptr;
}

ComponentRevoluteJoint* GameObject::GetRevoluteJoint() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::REVOLUTE_JOINT)
			return dynamic_cast<ComponentRevoluteJoint*>(components[i]);
	}

	return nullptr;
}

ComponentSliderJoint* GameObject::GetSliderJoint() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::PRISMATIC_JOINT)
			return dynamic_cast<ComponentSliderJoint*>(components[i]);
	}

	return nullptr;
}

ComponentSphericalJoint* GameObject::GetSphericalJoint() {

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == ComponentType::SPHERICAL_JOINT)
			return dynamic_cast<ComponentSphericalJoint*>(components[i]);
	}

	return nullptr;
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

void GameObject::UpdateBB() {

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

	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.25f, 1.0f, 0.0f, 1.0f);

	for (uint i = 0; i < obb.NumEdges(); i++)
	{
		glVertex3f(obb.Edge(i).a.x, obb.Edge(i).a.y, obb.Edge(i).a.z);
		glVertex3f(obb.Edge(i).b.x, obb.Edge(i).b.y, obb.Edge(i).b.z);
	}
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

}