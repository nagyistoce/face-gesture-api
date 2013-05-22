#include <opencv2/core/core.hpp>
#include "Module.h"
#include "Tracer.h"
#include "GlobalSettings.h"
#include "Configuration.h"


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
        std::cerr << "Warning: could not open module settings file, filename=\"" + configFileName + "\".\n";
}


Module::~Module(void)
{
    configurationFs_.release();
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


LONGLONG Module::GetTimeStamp(void) const
{
    return timeStamp_;
}


void Module::SetTimeStamp(LONGLONG timeStamp)
{
    timeStamp_ = timeStamp;
}


const cv::FileStorage& Module::GetModulConfigurationFs(void) const
{
    ASSERT(configurationFs_.isOpened());
    return configurationFs_;
}
