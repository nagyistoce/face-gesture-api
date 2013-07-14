#pragma once

#include <vector>
#include "FaceDef.h"

namespace face 
{
	//! Static FileSystem class.
	/*!
		\ingroup Core

		Provides file system related functionality as static functions.
	*/
	class FileSystem
	{
	public:
		//! Create a directory.
		/*!
		  \param[in] directory Name of the new directory.
		*/
		static void MakeDirectory(const std::string& directory);


		//! Return true, if the given path exists, and is the path of a file.
		/*!
		  \param[in] filePath Name of the file with path.
		  \return TRUE if the given file exists, otherwise it returns FALSE.
		*/
		static bool FileExists(const std::string& filePath);


		//! Return true, if the given path exists, and is the path of a directory.
		/*!
		  \param[in] directory Name of the directory to be checked.
		  \return TRUE if the given directory exists, otherwise it returns FALSE.
		*/
		static bool DirectoryExists(const std::string& directory);
	

		//! Return the names of files contained by the given directory
		/*!
		  \param[in] directory Name of the given directory.
		  \return List of filenames within the given directory.
		*/
		static std::vector<std::string> FileSystem::GetFiles(const std::string& directory);


		//! Return the names of subdirectories contained by the given directory.
		/*!
		  \param[in] directory Name of the given directory.
		  \return List of subdirectories within the given directory.
		*/
		static std::vector<std::string> FileSystem::GetSubdirectories(const std::string& directory);
	

	protected:
		/*! Get the content of a given directory.
		  \param[in] directory Name of the given directory.
		  \param[in] files Logical parameter to getting file content too.
		  \return List of subdirectories and optionally files within the given directory.
		*/
		static std::vector<std::string> GetDirectoryContent(const std::string& directory, bool files);


		/*! Convert a std::string to std::wstring.
		  \param[in] str std::string representation.
		  \return std::wstring representation.
		*/
		static std::wstring FileSystem::StringToWstring(const std::string& str);


		/*! Convert a std::wstring to std::string.
		  \param[in] str std::wstring representation.
		  \return std::string representation.
		*/
		static std::string FileSystem::WstringToString(const std::wstring& str);


	private:
		//! Constructor.
		FileSystem(void);


		//! Destructor.
		~FileSystem(void);
	};

}
