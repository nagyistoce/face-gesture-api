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
	//! Xml Reader class
	/*!
		\ingroup Core

		Wraps and extends the functionality of cv::FileStorage.
	*/
	class XmlReader
	{
	public:
		//! Constructor.
		/*!
		  \param[in] filePath Path of the xml.
		  \param[in] onErrorTerminate If it is TRUE then terminate on any of parsing error, otherwise just throw a warning message.
		*/
		XmlReader(const std::string& filePath, bool onErrorTerminate = false);


		//! Destructor.
		~XmlReader(void);


		//! Get the internal file storage of the xml.
		/*!
		  \return XmlReader::fileStorage_.
		*/
		cv::FileStorage& GetFileStorage(void);


		//! Validate the XML using XSD scheme definition.
		/*!
		  \param[in] schemaFilePath Path of the xml schema file
		*/
		void Validate(const std::string& schemaFilePath);


		//! Return true, if the xml exists.
		/*!
		  \return TRUE if the xml file exists, otherwise it returns FALSE.
		*/
		bool FileExists(void) const;



		//! Returns TRUE if the xml is valid.
		/*!
		  \return TRUE if the xml file is valid, otherwise it returns FALSE.
		*/
		bool IsValid(void) const;


		//! Returns TRUE if the xml is NOT valid.
		/*!
		  \return TRUE if the xml file is NOT valid, otherwise it returns FALSE.
		*/
		bool IsBroken(void) const;


		//!
		/*!
		  \param[in] tagName
		  \param[in] defaultValue
		  \return 
		*/
		std::string ReadString(const std::string& tagName, const std::string& defaultValue = "");


		//!
		/*!
		  \param[in] 
		  \return 
		*/
		int ReadInt(const std::string& tagName, int defaultValue = 0);


		//!
		/*!
		  \param[in] 
		  \return 
		*/
		float ReadFloat(const std::string& tagName, float defaultValue = 0);


		//!
		/*!
		  \param[in] 
		  \return 
		*/
		bool ReadBool(const std::string& tagName, bool defaultValue = 0);


		//!
		/*!
		  \param[in] 
		  \return 
		*/
		cv::FileNode GetFileNode(const std::string& path);


		//!
		/*!
		  \param[in] 
		  \return 
		*/
		static std::string ReadString(const cv::FileNode& fileNode, const std::string& tagName, const std::string& defaultValue);


		//!
		/*!
		  \param[in] 
		  \return 
		*/
		static int ReadInt(const cv::FileNode& fileNode, const std::string& tagName, int defaultValue);


		//!
		/*!
		  \param[in] 
		  \return 
		*/
		static float ReadFloat(const cv::FileNode& fileNode, const std::string& tagName, float defaultValue);


		//!
		/*!
		  \param[in] 
		  \return 
		*/
		static bool ReadBool(const cv::FileNode& fileNode, const std::string& tagName, bool defaultValue);


	private:
		//!
		/*!
		  \param[in] 
		  \return 
		*/
		void OnValidationFinished(void);


		cv::FileStorage* fileStorage_;	//!<
		std::string xmlPath_;			//!<
		bool exists_;					//!<
		bool valid_;					//!<
		std::string errorMsg_;			//!<
		bool onErrorTerminate_;			//!<
	};
}
