#pragma once

#include "p2Defs.h"

// -- Tools
#include <string>
#include <fstream>
#include <iomanip>
#include "JSON/json.hpp"
#include "MathGeoLib/include/MathGeoLib.h"

//Namespaces
using json = nlohmann::json;
using namespace std;

class Serializer {

public:

	Serializer();
	~Serializer();
	void Save(const char* file) const;
	json Load(const char* file);

	//-- Serialize
	void AddString(string name, string value);
	void AddInt(string name, int value);
	void AddUnsignedInt(string name, uint value);
	void AddFloat(string name, float value);
	void AddFloat3(string name, float3 vale);
	void AddQuaternion(string name, Quat value);
	void AddBool(string name, bool value);


	//-- Deserialize
	inline string GetString(string name) { return Object[name]; };
	inline int GetInt(string name) { return Object[name]; };
	inline uint GetUnsignedInt(string name) { return Object[name]; };
	inline float GetFloat(string name) { return Object[name]; };
	float3 GetFloatXYZ(string name);
	Quat GetQuaternion(string name);
	inline bool GetBool(string name) { return Object[name]; };

public: 
	
	json Object;
};