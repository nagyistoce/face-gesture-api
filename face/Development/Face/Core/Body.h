#pragma once

#include <map>
#include <vector>
#include <string>
#include <windows.h>
#include <tbb/flow_graph.h>
#include <opencv2/core/core.hpp>


#include "FaceDef.h"
#include "Timer.h"
#include "PortNameParser.h"

class Message;
class Configuration;

//! Abstract class for handling algorithms.
/*!
	\ingroup Core

	Class for specific distinct algorithms of the input processing.
	Each of them will be executed on independent threads.
*/
class Body
{
public:
	typedef std::map<int, std::string> PortNameMap;
	typedef std::map<std::string, Body*> PredecessorMap;

	//! Constructor.
	Body(const ConnectionElement& connectionElement);
	Body(const Body& other);

	//! Destructor.
	virtual ~Body(void);

	void operator= (const Body& other);

	//! Full name getter.
	const std::string& GetFullName(void) const;

	//! Module name getter.
	const std::string& GetModuleName(void) const;

	//! Instance name getter.
	const std::string& GetInstanceName(void) const;

	//!
	unsigned int GetTimestamp(void) const;


protected:
    virtual void DefinePorts(void) = 0;
    virtual void BeforeProcess(void);
    virtual void Process(void) = 0;
    virtual void AfterProcess(void);
    virtual void CheckPorts(const PredecessorMap& predecessorMap);

	bool HasOutput(void) const;
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


    std::string fullName_;          //!< Full name of the module: "ModuleName.instanceName".
    std::string moduleName_;        //!< Module name.
    std::string instanceName_;      //!< Instance name.

	unsigned int timestamp_;
    OutputType output_;
	cv::Mat outputFrame_;
    bool hasOutput_;

	PortNameMap portNameMap_;
    cv::FileStorage configurationFs_;
   

private: 
    Timer timer_;
};
