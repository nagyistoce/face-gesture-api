#include <opencv2/core/core.hpp>
#include "Body.h"
#include "Message.h"
#include "Tracer.h"
#include "GlobalSettings.h"
#include "Configuration.h"
#include "GarbageCollector.h"

using namespace std;


Body::Body(const VertexElement& vertexElement)
:	moduleName_(vertexElement.first.GetModuleName()),
    instanceName_(vertexElement.first.GetInstanceName()),
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

    hasSuccessor_ = vertexElement.second.size() > 0 ? true : false;
    timestamp_ = unsigned int(timer_.GetElapsedTimeInMicroSec());
}


Body::Body(const Body& other)
:	moduleName_(other.GetModuleName()),
	instanceName_(other.GetInstanceName()),
	fullName_(other.GetFullName()),
	hasSuccessor_(other.HasSuccessor()),
	timestamp_(other.GetTimestamp()),
	output_(NULL)
{
	//portNameMap_.insert(other.portNameMap_.begin(), other.portNameMap_.end());

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


bool Body::HasSuccessor(void) const
{
    return hasSuccessor_;
}


const cv::FileStorage& Body::GetModulConfigurationFs(void) const
{
    ASSERT(configurationFs_.isOpened());
    return configurationFs_;
}
