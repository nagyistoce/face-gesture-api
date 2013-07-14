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
	//! Configuration storage.
	/*!
		\ingroup Core

		Provides configuration-like accessible storage functionality.
		Stores boolean, string and numeric values that can be read from xml, and accessed by name.
	*/	
	class Configuration
	{
	public:
		//! Constructor.
		Configuration(void);


		//! Destructor.
		~Configuration(void);


		//! Loads a sequence of strings from an xml node.
		/*!
		  \param[in] fileNode FileNode from which the flags will be loaded.
		*/
		void LoadFlags(const cv::FileNode& fileNode);


		//! Loads key-value pairs from xml node.
		/*!
		  \param[in] fileNode fileNode FileNode from which the values will be loaded.
		*/
		void LoadValues(const cv::FileNode& fileNode);


		//! Return true if the given name is among the flags read from the xml.
		/*!
		  \param[in] name Name of the flag.
		  \return TRUE whether the flag has been set, otherwise FALSE.
		*/
		bool IsFlagSet(const std::string& name) const;


		//! Return true if there is any value is stored with the given name
		/*!
		  \param[in] name Name of the value.
		  \return TRUE whether the value has been set, otherwise FALSE.
		*/
		bool IsValueSet(const std::string& name) const;


		//! Return the value with the given name as string.
		/*!
		  \param[in] name Name of the value.
		  \param[in] def Default value, whether the value is not found.
		  \return The value if it was found otherwise it returns \param def.
		*/
		std::string GetValueAsString(const std::string& name, const std::string& def = "") const;


		//! return the value with the given name as integer.
		/*!
		  \param[in] name Name of the value.
		  \param[in] def Default value, whether the value is not found.
		  \return The value if it was found otherwise it returns \param def.
		*/
		int GetValueAsInt(const std::string& name, int def = 0) const;


		//! return the value with the given name floating point representation.
		/*!
		  \param[in] name Name of the value.
		  \param[in] def Default value, whether the value is not found.
		  \return The value if it was found otherwise it returns \param def.
		*/
		float GetValueAsFloat(const std::string& name, float def = 0.0f) const;


	private:
		//! Value representation.
		struct ValueDetails
		{
			std::string name;		//!< Name of the value.
			std::string stringVal;	//!< String representation.
			int intVal;				//!< Integer representation.
			float floatVal;			//!< Floating point representation.
		};


		//! Return the index of a given value.
		/*!
		  \param[in] name Name of the value.
		  \return The index of a value from Configuration::values_.
		*/
		int FindValueByName(const std::string& name) const;


		std::vector<std::string> flags_;	//!< Container for all of the flags.
		std::vector<ValueDetails> values_;	//!< Container for all of the values.
	};
}
