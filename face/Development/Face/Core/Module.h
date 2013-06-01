#pragma once

#include <map>
#include <vector>
#include <string>
#include <windows.h>
#include <opencv2/core/core.hpp>

#include "FaceDef.h"
#include "Timer.h"

class Message;
class Configuration;


//! Abstract class for handling algorithms.
/*!
	\ingroup Core

	Class for specific distinct algorithms of the input processing.
	Each of them will be executed on independent threads.
*/
class Module
{
public:
    typedef Message* OutputType;

    //! Full name getter.
    const std::string& GetFullName(void) const;

    //! Module name getter.
    const std::string& GetModuleName(void) const;

    //! Instance name getter.
    const std::string& GetInstanceName(void) const;

    //!
    unsigned int GetTimestamp(void) const;


protected:
    //! Constructor.
    Module(const std::string& moduleName, const std::string& instanceName);

    //! Destructor.
    virtual ~Module(void);

    virtual void BeforeProcess(void);
    virtual void Process(void) = 0;
    virtual void AfterProcess(OutputType message);

    void RefreshTimestamp(void);

    //!
    void SetTimestamp(unsigned int timestamp);


    //! Open the module configuration xml file.
	/*!
		The module configuration xml file has filename 
		"Testing/Futtatas/Input/Modules/MODULENAME/Settings.CONFIG.xml"
		where MODULENAME is the name of the module type and CONFIG is the
		configuration name given in the process configuration.  If CONFIG
		is empty, the dot before it is omitted.
	*/
	const cv::FileStorage& GetModulConfigurationFs(void) const;


private:
    std::string fullName_;          //!< Full name of the module: "ModuleName.instanceName".
    std::string moduleName_;        //!< Module name.
    std::string instanceName_;      //!< Instance name.

    unsigned int timestamp_;
    cv::FileStorage configurationFs_;
    Timer timer_;
};
