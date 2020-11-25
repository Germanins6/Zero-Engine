#include "Serialization.h"


Serializer::Serializer() {

}

Serializer::~Serializer(){

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

string GetString(string name) {

}