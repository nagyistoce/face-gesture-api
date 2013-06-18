#pragma once

#include <string>
#include <vector>
#include <tbb/mutex.h>

#include "FaceDef.h"
#include "Configuration.h"

#define GlobalSettingsConstPtr GlobalSettings::GetConstInstance()
#define GlobalSettingsPtr GlobalSettings::GetInstance()


class GlobalSettings
:   public Configuration
{
public:
	//! Useable directory paths
	/*! 
		Whenever the path of a file is needed inside the program, 
		it must be constructed by extending one of the here-defined
		paths with the relative path part (relatisubdirectories and filename).

		The directory names already contain an ending slash.
	*/
	struct Directories 
	{
		std::string	data;
		std::string	resource;
		std::string input;
		std::string output;
		std::string source;
		std::string moduleSettings;
	};

	static GlobalSettings* GetInstance(void);
	static const GlobalSettings* GetConstInstance(void);

	/*! Get the available directories

	@return all the directory paths in a Directories struct
	*/
	const Directories& GetDirectories(void) const;
	
	/*! Get the settings directory of a module

	The modules should get their settings directory path through this function.
	This way less environment-specific code is hardwired to the modules.

	@param module the name of the module
	@return the settings directory path of the module with an ending slash
	*/
	std::string GetModuleSettingsDirectory(const std::string& module) const;

    const std::string& GetProcessXml(void) const;

	/*! Initialize the directory list

	Updates the accessible directories by reading the working directory
	from the give path.

	Should be called at the very beginning of the program.
	*/
	void SetLocalSettingsFilePath(const std::string& filePath);

	/*! Set process xml filepath

	The runtime configuration is defined in the process xml, will be read from there.
	*/
	void SetProcessXml(const std::string& processXml);

	
private:
    static GlobalSettings* instance_;
    static tbb::mutex mutex_;

	GlobalSettings(void);

	Directories directories_;
    std::string processXml_;
};
