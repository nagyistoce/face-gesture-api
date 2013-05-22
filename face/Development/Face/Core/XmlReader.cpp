#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>

#include "XmlReader.h"
#include "FileSystem.h"
#include "ExceptionDescriptor.h"
#include "Tracer.h"


using namespace std;
using namespace cv;


XmlReader::XmlReader(const string& filePath, bool onErrorTerminate)
:	valid_(false),
	exists_(false),
	onErrorTerminate_(onErrorTerminate),
	xmlPath_(filePath),
	fileStorage_(NULL)
{	
	if (FileSystem::FileExists(filePath))
	{
		ifstream testIn(filePath.c_str());
		if (testIn.is_open()) 
			exists_ = testIn.peek() != std::ifstream::traits_type::eof();
		testIn.close();
	}

	if (exists_)
	{
		try
		{
			fileStorage_ = new cv::FileStorage(filePath, FileStorage::READ);
			if(fileStorage_->isOpened())
				valid_ = true;
			else
				errorMsg_ = "file could not be opened";
		}
		catch(...)
		{
			errorMsg_ = "syntax error";
		}
	}
	else
	{
		errorMsg_ = "file does not exist";
	}

	OnValidationFinished();
}


XmlReader::~XmlReader(void)
{
	if (fileStorage_)
		delete fileStorage_;
}


FileStorage& XmlReader::GetFileStorage(void)
{
	return *fileStorage_;
}


void XmlReader::Validate(const string& schemaFilePath)
{	
	//TODO
	LOG("TODO - validate XML");
	OnValidationFinished();
}


bool XmlReader::FileExists(void) const
{
	return exists_;
}


bool XmlReader::IsValid(void) const
{
	return valid_;
}


bool XmlReader::IsBroken(void) const
{
	return valid_ == false;
}


string XmlReader::ReadString(const string& path, const string& defaultValue)
{
	if (valid_ == false)
		return defaultValue;

	FileNode node = GetFileNode(path);

	if (node.isString())
		return std::string(node);
	else
		return defaultValue;
}


int XmlReader::ReadInt(const string& path, int defaultValue)
{
	if (valid_ == false)
		return defaultValue;

	FileNode node = GetFileNode(path);

	if (node.isInt())
		return int(node);
	else
		return defaultValue;
}


float XmlReader::ReadFloat(const string& path, float defaultValue)
{
	if (valid_ == false)
		return defaultValue;

	FileNode node = GetFileNode(path);

	if (node.isReal())
		return float(node);
	else
		return defaultValue;
}


bool XmlReader::ReadBool(const string& path, bool defaultValue)
{
	if (valid_ == false)
		return defaultValue;

	FileNode node = GetFileNode(path);

	if (node.isString())
		return string(node) == "true";
	else
		return defaultValue;
}


string XmlReader::ReadString(const FileNode& fileNode, const string& tagName, const string& defaultValue)
{
	if(fileNode[tagName].isString())
		return std::string(fileNode[tagName]);
	else
		return std::string(defaultValue);
}


int XmlReader::ReadInt(const FileNode& fileNode, const string& tagName, int defaultValue)
{
	if (fileNode[tagName].isInt()) 
		return int(fileNode[tagName]);
	else
		return defaultValue;
}


float XmlReader::ReadFloat(const FileNode& fileNode, const string& tagName, float defaultValue)
{
	if (fileNode[tagName].isReal()) 
		return float(fileNode[tagName]);
	else
		return defaultValue;
}


bool XmlReader::ReadBool(const FileNode& fileNode, const string& tagName, bool defaultValue)
{
	if (fileNode[tagName].isString())
		return std::string(fileNode[tagName]) == "true";
	else
		return defaultValue;
}


FileNode XmlReader::GetFileNode(const std::string& path)
{
	ASSERT(path.size() > 0);
	
	bool first = true;
	FileNode node;

	char* tmp = new char[path.size() + 1];
	const char* runner = path.c_str();

	while (true)
	{
		int i=0;
		while (*runner && *runner != '/')
		{
			tmp[i] = *runner;
			i++;
			runner++;
		}
		tmp[i] = '\0';
		
		if (first)
			node = (*fileStorage_)[string(tmp)];
		else
			node = node[string(tmp)];
		first = false;

		if (*runner == '\0' || node.isNone())
			break;

		runner++;
	}

	delete[] tmp;
	return node;
}


void XmlReader::OnValidationFinished(void)
{
	if (valid_)
		return;
	
	string message = "XML validation fails for " + xmlPath_ + ". Message: " + errorMsg_;
	if (onErrorTerminate_)
		throw ExceptionError(message);
	else
		LOG("Warning - " + message);
}
