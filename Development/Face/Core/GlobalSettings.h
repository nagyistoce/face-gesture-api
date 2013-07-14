#pragma once

#include <string>
#include <vector>
#include <tbb/mutex.h>

#include "FaceDef.h"
#include "Configuration.h"


namespace face 
{
	#define GlobalSettingsConstPtr GlobalSettings::GetConstInstance()
	#define GlobalSettingsPtr GlobalSettings::GetInstance()

	//! Singleton class for handling directory's structure.
	/*! 
		\ingroup Core
	*/
	class GlobalSettings
	:   public Configuration
	{
	public:
		//! Useable directory paths
		/*! 
			Whenever the path of a file is needed inside the program, 
			it must be constructed by extending one of the here-defined
			paths with the relative path part (subdirectories and filename).

			The directory names already contain an ending slash.
		*/
		struct Directories 
		{
			std::string	data;			//!< Main directory of test data.
			std::string	resource;		//!< Resource directory (3rdParty libraries).
			std::string input;			//!< Input data directory.
			std::string output;			//!< Output data directory.
			std::string source;			//!< Source settings directory.
			std::string moduleSettings;	//!< Module settings directory.
			std::string videoInput;		//!< Video input directory.
		};


		//! Getter of a static instance.
		static GlobalSettings* GetInstance(void);


		//! Const getter of a static instance.
		static const GlobalSettings* GetConstInstance(void);


		//! Get the available directories
		/*!
			\return all the directory paths in a Directories struct
		*/
		const Directories& GetDirectories(void) const;
	

		//! Get the settings directory of a module
		/*!
			The modules should get their settings directory path through this function.
			This way less environment-specific code is hardwired to the modules.

			\param[in] module The name of the module.
			\return The settings directory path of the module with an ending slash.
		*/
		std::string GetModuleSettingsDirectory(const std::string& module) const;


		//! Get the settings directory of a module
		/*!
			The modules should get their settings directory path through this function.
			This way less environment-specific code is hardwired to the modules.

			\return The settings directory path of the module with an ending slash.
		*/
		const std::string& GetProcessXml(void) const;


		//! Initialize the directory list
		/*!
			Updates the accessible directories by reading the working directory
			from the give path.

			Should be called at the very beginning of the program.

			\param[in] filePath The name of the local settings file.
		*/
		void SetLocalSettingsFilePath(const std::string& filePath);


		//! Set process xml filepath
		/*!
			The runtime configuration is defined in the process xml, will be read from there.

			\param[in] processXml The name of the process xml file.
		*/
		void SetProcessXml(const std::string& processXml);

	
	private:
		static GlobalSettings* instance_;	//!< Static instance of GlobalSettings.
		static tbb::mutex mutex_;			//!< Mutex for creating GlobalSettings::instance_.


		//! Constructor.
		GlobalSettings(void);

		//! Destructor.
		~GlobalSettings(void);


		Directories directories_;			//!< Container for the used paths.
		std::string processXml_;			//!< Process xml file name.
	};
}
