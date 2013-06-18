#include <fstream>
#include <sstream>
#include <cstddef>

#include "GlobalSettings.h"
#include "ExceptionDescriptor.h"
#include "Tracer.h"


GlobalSettings* GlobalSettings::instance_ = NULL;
tbb::mutex GlobalSettings::mutex_;


using namespace std;


GlobalSettings::GlobalSettings(void)
{
}


GlobalSettings* GlobalSettings::GetInstance(void)
{
    if(instance_ == NULL)
    {
        tbb::mutex::scoped_lock lock(mutex_);
        if(instance_ == NULL)
            instance_ = new GlobalSettings();
    }
    return instance_;
}


const GlobalSettings* GlobalSettings::GetConstInstance(void)
{
    return GetInstance();
}


const GlobalSettings::Directories& GlobalSettings::GetDirectories(void) const
{
    return directories_;
}


const string& GlobalSettings::GetProcessXml(void) const
{
    return processXml_;
}


string GlobalSettings::GetModuleSettingsDirectory(const string& module) const
{
    return directories_.moduleSettings + module + "/";
}


void GlobalSettings::SetLocalSettingsFilePath(const string& filePath)
{
    ifstream fileStream(filePath.c_str());

    if (!fileStream.is_open()) 
        throw ExceptionError("Local settings file '" + filePath + "' does not exist!");

    getline(fileStream, directories_.data);
    fileStream.close();

    processXml_ = "Process.default.xml";

    directories_.input = directories_.data + "Input/";
    directories_.output = directories_.data + "Output/";
    directories_.resource = directories_.data + "Resources/";
    directories_.source = directories_.input + "Source/";
    directories_.moduleSettings = directories_.input + "Modules/";

    Tracer::GetInstance()->SetOutputDirectory(directories_.output);
}


void GlobalSettings::SetProcessXml(const std::string& processXml)
{
    processXml_ = directories_.input + processXml;
}
