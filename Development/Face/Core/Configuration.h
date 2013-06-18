#pragma once

#include <string>
#include <vector>
#include "FaceDef.h"


namespace cv
{
	class FileNode;
}

namespace face 
{

//! Configuration storage
/*!
	Provides configuration-like accessible storage functionality

	Stores boolean, string and numeric values that can be read from xml, and accessed by name.
*/	
class Configuration
{
public:
	Configuration(void);
	~Configuration(void);

	//! Loads a sequence of strings from an xml node
	void LoadFlags(const cv::FileNode& fileNode);

	//! Loads key-value pairs from xml node
	void LoadValues(const cv::FileNode& fileNode);

	//! return true if the given name is among the flags read from the xml
	bool IsFlagSet(const std::string& name) const;

	//! return true if there is any value is stored with the given name
	bool IsValueSet(const std::string& name) const;

	//! return the value with the given name
	std::string GetValueAsString(const std::string& name, const std::string& def = "") const;

	//! return the value with the given name 
	int GetValueAsInt(const std::string& name, int def = 0) const;

	//! return the value with the given name
	float GetValueAsFloat(const std::string& name, float def = 0.0f) const;


private:
	struct ValueDetails
	{
		std::string name;
		std::string stringVal;
		int intVal;
		float floatVal;
	};

	int FindValueByName(const std::string& name) const;

	std::vector<std::string> flags_;
	std::vector<ValueDetails> values_;
};

}
