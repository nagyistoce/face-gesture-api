#pragma once

#include <string>
#include <opencv2/core/core.hpp>
#include <tbb/flow_graph.h>

#include "FaceDef.h"
#include "Timer.h"
#include "PortNameParser.h"


class Message;

//! Abstract class for handling algorithms.
/*!
	\ingroup Core

	Class for specific distinct algorithms of the input processing.
	Each of them will be executed on independent threads.
*/
class Body
{
public:
    typedef Message* OutputType;
    typedef Message* InputType1;
    typedef tbb::flow::continue_msg ContinueMessage;
    typedef std::tuple<ContinueMessage, ContinueMessage> ContinueMessagePair;

	//! Constructor.
	Body(const VertexElement& vertexElement);
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
    virtual void BeforeProcess(void);
    virtual void Process(void) = 0;
    virtual void AfterProcess(void);

	bool HasSuccessor(void) const;

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
    bool hasSuccessor_;

    cv::FileStorage configurationFs_;
   

private: 
    Timer timer_;
};
