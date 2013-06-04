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
class Module
{
public:
    //typedef Message* OutputType;
    typedef std::map<std::string, Module*> PredecessorMap;
   // typedef tbb::flow::broadcast_node<tbb::flow::continue_msg> ContinueNodeType;

    virtual void CreateConnection(PredecessorMap& predecessorMap);

    //! Full name getter.
    const std::string& GetFullName(void) const;

    //! Module name getter.
    const std::string& GetModuleName(void) const;

    //! Instance name getter.
    const std::string& GetInstanceName(void) const;

    //!
    unsigned int GetTimestamp(void) const;

    bool HasOutput(void) const;


Module& operator=(const Module& /*other*/) {
        return *this;
    }

protected:
    typedef std::map<int, std::string> PortNameMap;

    //struct ContinueNodeBody 
    //{
    //    ContinueNodeBody(void);
    //    void operator() (tbb::flow::continue_msg) const;
    //};


    //! Constructor.
    Module(const ConnectionElement& connectionElement);
    Module(const Module& other);

    //! Destructor.
    virtual ~Module(void);

    virtual void DefinePorts(void) = 0;
    virtual void BeforeProcess(void);
    virtual void Process(void) = 0;
    virtual void AfterProcess(void);
    virtual void CheckInputMessages(void);
    
    virtual void CheckPorts(const PredecessorMap& predecessorMap);

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

    Message* output_;
    bool hasOutput_;
    unsigned int timestamp_;
    cv::FileStorage configurationFs_;
    cv::Mat outputFrame_;
    PortNameMap portNameMap_;
    //tbb::flow::graph& graph_;
    //ContinueNodeType* continueNode_;


private: 
    Timer timer_;
};
