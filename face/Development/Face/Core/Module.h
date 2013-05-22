#pragma once

#include <map>
#include <vector>
#include <string>
#include <windows.h>
#include <opencv2/core/core.hpp>
#include "tbb/flow_graph.h"

#include "FaceDef.h"

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
    //! Full name getter.
    const std::string& GetFullName(void) const;

    //! Module name getter.
    const std::string& GetModuleName(void) const;

    //! Instance name getter.
    const std::string& GetInstanceName(void) const;

    //!
    LONGLONG GetTimeStamp(void) const;

    //!
    void SetTimeStamp(LONGLONG timeStamp);

protected:
    //! Constructor.
    Module(const std::string& moduleName, const std::string& instanceName);

    //! Destructor.
    virtual ~Module(void);

    virtual bool operator() (Message* input) = 0;

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
    std::string configurationName_;	//!< Configuration name
    LONGLONG timeStamp_;
    cv::FileStorage configurationFs_;
};
