#include <opencv2/core/core.hpp>
#include "Module.h"
#include "Message.h"
#include "Tracer.h"
#include "GlobalSettings.h"
#include "Configuration.h"
#include "GarbageCollector.h"

using namespace std;


Module::Module(const string& moduleName, const string& instanceName)
:	moduleName_(moduleName),
    instanceName_(instanceName)
{
    fullName_ = ((instanceName_ == "") ? moduleName_ : (moduleName_ + '.' + instanceName_));

    string configFileName = GlobalSettingsConstPtr->GetDirectories().moduleSettings + "/" + moduleName_ + 
        "/Settings." + instanceName_ + ".xml";

    configurationFs_.open(configFileName, cv::FileStorage::READ, "UTF-8");
    if(!configurationFs_.isOpened())
    {
        std::cerr << "Warning: could not open module settings file, filename=\"" + configFileName + "\".\n";
    }

    timestamp_ = unsigned int(timer_.GetElapsedTimeInMicroSec());
}


Module::~Module(void)
{
    configurationFs_.release();
}


void Module::BeforeProcess(void)
{
    timestamp_ = unsigned int(timer_.GetElapsedTimeInMicroSec());
}


void Module::AfterProcess(OutputType message)
{
    if(message != NULL)
    {
        GarbageCollectorPtr->PushNewOutput(message, fullName_);
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


void Module::SetTimestamp(unsigned int timestamp)
{
    timestamp_ = timestamp;
}


const cv::FileStorage& Module::GetModulConfigurationFs(void) const
{
    ASSERT(configurationFs_.isOpened());
    return configurationFs_;
}
