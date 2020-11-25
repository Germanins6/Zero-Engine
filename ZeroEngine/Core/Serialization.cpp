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

//Deserialize functions

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