#include "Serialization.h"


Serializer::Serializer() {

}

Serializer::~Serializer(){

}

void Serializer::Save(const char* file) const{

	ofstream stream(file, ofstream::out);
	stream << setw(2) << Object << endl;
	stream.close();
}

json Serializer::Load(const char* file) {

	json new_json;

	//Open file
	ifstream stream(file, ifstream::in);

	//Write data into new json
	new_json = json::parse(stream);

	//Return json
	return new_json;
}

//Serialize functions

//----------------
void Serializer::AddString(string name, string value) {
	Object[name] = value;
}

void Serializer::AddInt(string name, int value) {
	Object[name] = value;
}

void Serializer::AddUnsignedInt(string name, uint value) {
	Object[name] = value;
}

void Serializer::AddFloat(string name, float value) {
	Object[name] = value;
}

void Serializer::AddFloat3(string name, float3 value) {
	Object[name] = { value.x,value.y,value.z };
}

void Serializer::AddQuaternion(string name, Quat value) {
	Object[name] = { value.x, value.y, value.z, value.w };
}

void Serializer::AddBool(string name, bool value) {
	Object[name] = value;
}

//Objects
void Serializer::AddStringObj(string name, string value, string obj_name) {
	Object[obj_name][name] = value;
}

void Serializer::AddIntObj(string name, int value, string obj_name) {
	Object[obj_name][name] = value;
}

void Serializer::AddUnsignedIntObj(string name, uint value, string obj_name) {
	Object[obj_name][name] = value;
}

void Serializer::AddFloatObj(string name, float value, string obj_name) {
	Object[obj_name][name] = value;
}

void Serializer::AddFloat3Obj(string name, float3 value, string obj_name) {
	Object[obj_name][name] = { value.x,value.y,value.z };
}

void Serializer::AddQuaternionObj(string name, Quat value, string obj_name) {
	Object[obj_name][name] = { value.x, value.y, value.z, value.w };
}

void Serializer::AddBoolObj(string name, bool value, string obj_name) {
	Object[obj_name][name] = value;
}

//Deserialize functions
//-------------
float3 Serializer::GetFloatXYZ(string name) {
	
	float3 xyz;

	for (int i = 0; i < Object[name].size(); i++) 
		xyz[i] = Object[name].at(i);
		
	return xyz;
}

Quat Serializer::GetQuaternion(string name) {

	Quat quaternion;

	quaternion.x = Object[name].at(0);
	quaternion.y = Object[name].at(1);
	quaternion.z = Object[name].at(2);
	quaternion.w = Object[name].at(3);

	return quaternion;
}

//Objects
float3 Serializer::GetFloatXYZObj(string name, string obj_name) {

	float3 xyz;

	for (int i = 0; i < Object[obj_name][name].size(); i++)
		xyz[i] = Object[obj_name][name].at(i);

	return xyz;
}

Quat Serializer::GetQuaternionObj(string name, string obj_name) {

	Quat quaternion;

	quaternion.x = Object[obj_name][name].at(0);
	quaternion.y = Object[obj_name][name].at(1);
	quaternion.z = Object[obj_name][name].at(2);
	quaternion.w = Object[obj_name][name].at(3);

	return quaternion;
}