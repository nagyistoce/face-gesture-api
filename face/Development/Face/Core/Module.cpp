#include <opencv2/core/core.hpp>
#include "Module.h"
#include "Message.h"
#include "Tracer.h"
#include "GlobalSettings.h"
#include "Configuration.h"
#include "GarbageCollector.h"

using namespace std;


//Module::ContinueNodeBody::ContinueNodeBody(void)
//{
//}
//
//
//void Module::ContinueNodeBody::operator() (tbb::flow::continue_msg) const
//{
//}


Module::Module(const ConnectionElement& connectionElement)
:	moduleName_(connectionElement.first.GetModuleName()),
    instanceName_(connectionElement.first.GetInstanceName()),
    output_(NULL)/*,
    continueNode_(NULL)*/
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


Module::Module(const Module& other)
{
    output_ = other.output_ ? new Message(*other.output_) : NULL;
    //continueNode_ = other.continueNode_ ? new Module::ContinueNodeType(*other.continueNode_) : NULL;

    if(!other.outputFrame_.empty())
        other.outputFrame_.copyTo(outputFrame_);

    fullName_ = other.fullName_;
    moduleName_ = other.moduleName_;
    instanceName_ = other.instanceName_;
    hasOutput_ = other.hasOutput_;
    timestamp_ = other.timestamp_;
    configurationFs_ = other.configurationFs_;
    portNameMap_ = other.portNameMap_;
}


Module::~Module(void)
{
    configurationFs_.release();
    delete output_;
   // delete continueNode_;
}


void Module::BeforeProcess(void)
{
    timestamp_ = unsigned int(timer_.GetElapsedTimeInMicroSec());
}


void Module::AfterProcess(void)
{
    if(output_ != NULL)
    {
        GarbageCollectorPtr->PushNewOutput(output_, fullName_);
    }

    CheckInputMessages();
}


void Module::CheckInputMessages(void)
{

}


void Module::CreateConnection(Module::PredecessorMap& predecessorMap)
{
    DefinePorts();
    CheckPorts(predecessorMap);
}


void Module::CheckPorts(const Module::PredecessorMap& predecessorMap)
{
    for(Module::PredecessorMap::const_iterator itPred = predecessorMap.begin(); itPred != predecessorMap.end(); itPred++)
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
        for(Module::PredecessorMap::const_iterator itPred = predecessorMap.begin(); itPred != predecessorMap.end(); itPred++)
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


const string& Module::GetFullName(void) const
{
    return fullName_;
}


const string& Module::GetModuleName(void) const
{
    return moduleName_;
}


const string& Module::GetInstanceName(void) const
{
    return instanceName_;
}


unsigned int Module::GetTimestamp(void) const
{
    return timestamp_;
}


bool Module::HasOutput(void) const
{
    return hasOutput_;
}


void Module::SetTimestamp(unsigned int timestamp)
{
    timestamp_ = timestamp;
}


const cv::FileStorage& Module::GetModulConfigurationFs(void) const
{
    ASSERT(configurationFs_.isOpened());
    return configurationFs_;
}
