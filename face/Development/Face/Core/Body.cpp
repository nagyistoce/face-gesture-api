#include <opencv2/core/core.hpp>
#include "Body.h"
#include "Message.h"
#include "Tracer.h"
#include "GlobalSettings.h"
#include "Configuration.h"
#include "GarbageCollector.h"

using namespace std;


Body::Body(const ConnectionElement& connectionElement)
:	moduleName_(connectionElement.first.GetModuleName()),
    instanceName_(connectionElement.first.GetInstanceName()),
    output_(NULL)
{
    fullName_ = ((instanceName_ == "") ? moduleName_ : (moduleName_ + '.' + instanceName_));

    string configFileName = GlobalSettingsConstPtr->GetDirectories().moduleSettings + "/" + moduleName_ + 
        "/Settings." + instanceName_ + ".xml";

    configurationFs_.open(configFileName, cv::FileStorage::READ, "UTF-8");
    if(!configurationFs_.isOpened())
    {
        std::cerr << "Warning: could not open module settings file, filename=\"" + configFileName + "\".\n";
    }

    hasOutput_ = connectionElement.second.size() > 0 ? true : false;
    timestamp_ = unsigned int(timer_.GetElapsedTimeInMicroSec());
}


Body::Body(const Body& other)
:	moduleName_(other.GetModuleName()),
	instanceName_(other.GetInstanceName()),
	fullName_(other.GetFullName()),
	hasOutput_(other.HasOutput()),
	timestamp_(other.GetTimestamp()),
	output_(NULL)
{
	portNameMap_.insert(other.portNameMap_.begin(), other.portNameMap_.end());

    if(other.output_)
	{
		output_ = new Message(*other.output_);
	}

    if(!other.outputFrame_.empty())
	{
        other.outputFrame_.copyTo(outputFrame_);
	}

	string configFileName = GlobalSettingsConstPtr->GetDirectories().moduleSettings + "/" + moduleName_ + 
		"/Settings." + instanceName_ + ".xml";

	configurationFs_.open(configFileName, cv::FileStorage::READ, "UTF-8");
	if(!configurationFs_.isOpened())
	{
		std::cerr << "Warning: could not open module settings file, filename=\"" + configFileName + "\".\n";
	}
}


Body::~Body(void)
{
    configurationFs_.release();
    delete output_;
}


void Body::operator= (const Body& other) 
{
	return;
}


void Body::BeforeProcess(void)
{
    timestamp_ = unsigned int(timer_.GetElapsedTimeInMicroSec());
}


void Body::AfterProcess(void)
{
    if(output_ != NULL)
    {
        GarbageCollectorPtr->PushNewOutput(output_, fullName_);
    }

    //CheckInputMessages();
}


void Body::CheckPorts(const Body::PredecessorMap& predecessorMap)
{
	DefinePorts();

    for(Body::PredecessorMap::const_iterator itPred = predecessorMap.begin(); itPred != predecessorMap.end(); itPred++)
    {
        bool foundPredPort = false;
        for(PortNameMap::const_iterator itPnm = portNameMap_.begin(); itPnm != portNameMap_.end(); itPnm++)
        {
            if(itPnm->second.empty())
                continue;

            if(itPred->first == itPnm->second)
            {
                foundPredPort = true;
                break;
            }
        }

        if(!foundPredPort)
            CV_Error(-1, "Error: Use of undefined port in process.xml: " + GetFullName() + ":" + itPred->first + ".");
    }

    for(PortNameMap::const_iterator itPnm = portNameMap_.begin(); itPnm != portNameMap_.end(); itPnm++)
    {
        if(itPnm->second.empty())
            continue;

        bool foundPort = false;
        for(Body::PredecessorMap::const_iterator itPred = predecessorMap.begin(); itPred != predecessorMap.end(); itPred++)
        {
            if(itPred->first == itPnm->second)
            {
                foundPort = true;
                break;
            }
        }

        if(!foundPort)
            CV_Error(-1, "Error: Missing port connection in process.xml: " + GetFullName() + ":" + itPnm->second + ".");
    }    
}


const string& Body::GetFullName(void) const
{
    return fullName_;
}


const string& Body::GetModuleName(void) const
{
    return moduleName_;
}


const string& Body::GetInstanceName(void) const
{
    return instanceName_;
}


unsigned int Body::GetTimestamp(void) const
{
    return timestamp_;
}


bool Body::HasOutput(void) const
{
    return hasOutput_;
}


void Body::SetTimestamp(unsigned int timestamp)
{
    timestamp_ = timestamp;
}


const cv::FileStorage& Body::GetModulConfigurationFs(void) const
{
    ASSERT(configurationFs_.isOpened());
    return configurationFs_;
}
