#include <opencv2/core/core.hpp>
#include "Configuration.h"
#include "Tracer.h"


using namespace std;
using namespace cv;


namespace face 
{
	Configuration::Configuration(void)
	{
	}


	Configuration::~Configuration(void)
	{
	}


	void Configuration::LoadFlags(const FileNode& fileNode)
	{
		if (fileNode.isSeq() == false)
			return;

		for (unsigned int i=0; i<fileNode.size(); i++)
			flags_.push_back(string(fileNode[i]));
	}


	void Configuration::LoadValues(const FileNode& fileNode)
	{
		if (fileNode.isNone())
			return;

		for(FileNodeIterator it = fileNode.begin(); it != fileNode.end(); it++)
		{
			ValueDetails details;
			details.name = (*it).name();
			details.stringVal = string(*it);
			details.intVal = int(*it);
			details.floatVal = float(*it);
			values_.push_back(details);
		}
	}


	bool Configuration::IsFlagSet(const std::string& name) const
	{
		for (unsigned int i=0; i<flags_.size(); i++)
			if (flags_[i] == name)
				return true;
		return false;
	}


	bool Configuration::IsValueSet(const std::string& name) const
	{
		return FindValueByName(name) >= 0;
	}


	string Configuration::GetValueAsString(const string& name, const string& def) const
	{
		int index = FindValueByName(name);
		if (index < 0)
			return def;
		return values_[index].stringVal;
	}


	int Configuration::GetValueAsInt(const string& name, int def) const
	{
		int index = FindValueByName(name);
		if (index < 0)
			return def;
		return values_[index].intVal;
	}


	float Configuration::GetValueAsFloat(const string& name, float def) const
	{
		int index = FindValueByName(name);
		if (index < 0)
			return def;
		return values_[index].floatVal;
	}


	int Configuration::FindValueByName(const std::string& name) const
	{
		for(unsigned int i=0; i<values_.size(); i++)
			if(values_[i].name == name)
				return i;
		return -1;
	}
}
