#pragma once

#include <vector>
#include "FaceDef.h"


//! Static FileSystem class
/*!
	Provides file system related functionality as static functions
*/
class FileSystem
{
public:
	//! Create a directory
	static void MakeDirectory(const std::string& directory);

	//! return true, if the given path exists, and is the path of a file
	static bool FileExists(const std::string& filePath);

	//! return true, if the given path exists, and is the path of a directory
	static bool DirectoryExists(const std::string& directory);
	
	//! return the names of files contained by the given directory
	static std::vector<std::string> FileSystem::GetFiles(const std::string& directory);

	//! return the names of subdirectories contained by the given directory
	static std::vector<std::string> FileSystem::GetSubdirectories(const std::string& directory);
	
protected:
	static std::vector<std::string> GetDirectoryContent(const std::string& directory, bool files);

	static std::wstring FileSystem::StringToWstring(const std::string& str);
	static std::string FileSystem::WstringToString(const std::wstring& str);

private:
    FileSystem(void);
    ~FileSystem(void);
};
