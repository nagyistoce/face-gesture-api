#pragma once

#include <string>

#include "ExceptionDescriptor.h"
#include "Tracer.h"


namespace cv
{
	class FileStorage;
	class FileNode;
}


namespace face 
{

//! Xml Reader
/*!
	Wraps and extends the functionality of cv::FileStorage.
*/
class XmlReader
{
public:
	XmlReader(const std::string& filePath, bool onErrorTerminate = false);
	~XmlReader(void);

	cv::FileStorage& GetFileStorage(void);

	//! Validate the XML using XSD scheme definition
	void Validate(const std::string& schemaFilePath);

	bool FileExists(void) const;
	//! Returns if the xml is valid
	bool IsValid(void) const;

	//! Returns if the xml is NOT valid
	bool IsBroken(void) const;

	std::string ReadString(const std::string& tagName, const std::string& defaultValue = "");
	int ReadInt(const std::string& tagName,int defaultValue = 0);
	float ReadFloat(const std::string& tagName,float defaultValue = 0);
	bool ReadBool(const std::string& tagName,bool defaultValue = 0);
	cv::FileNode GetFileNode(const std::string& path);

	static std::string ReadString(const cv::FileNode& fileNode, const std::string& tagName, const std::string& defaultValue);
	static int ReadInt(const cv::FileNode& fileNode, const std::string& tagName, int defaultValue);
	static float ReadFloat(const cv::FileNode& fileNode, const std::string& tagName, float defaultValue);
	static bool ReadBool(const cv::FileNode& fileNode, const std::string& tagName, bool defaultValue);


private:
	void OnValidationFinished(void);

	cv::FileStorage* fileStorage_;
	
	std::string xmlPath_;
	bool exists_;
	bool valid_;
	std::string errorMsg_;
	bool onErrorTerminate_;
};

}
