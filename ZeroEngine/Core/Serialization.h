#pragma once

#include "p2Defs.h"

// -- Tools
#include <string>
#include "JSON/json.hpp"

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
	void AddBool(string name, bool value);

	//-- Deserialize
	string GetString();
	int GetInt();
	uint GetUnsignedInt();
	float GetFloat();
	bool GetBool();


};