#pragma once

#include "p2Defs.h"

// -- Tools
#include <string>
#include "JSON/json.hpp"
#include "MathGeoLib/include/MathGeoLib.h"

//Namespaces
using json = nlohmann::json;
using namespace std;

class Serializer {

public:

	Serializer();
	~Serializer();

	//-- Serialize
	void AddString(string name, string value);
	void AddInt(string name, int value);
	void AddUnsignedInt(string name, uint value);
	void AddFloat(string name, float value);
	void AddFloat3(string name, float3 vale);
	void AddQuaternion(string name, Quat value);
	void AddBool(string name, bool value);


	//-- Deserialize
	string GetString(string name);
	int GetInt(string name);
	uint GetUnsignedInt(string name);
	float GetFloat(string name);
	float3 GetFloatXYZ(string name);
	Quat GetQuaternion(string name);
	bool GetBool(string name);


public:
	json Object;
};