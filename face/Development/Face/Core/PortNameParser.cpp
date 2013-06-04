#include "PortNameParser.h"

using namespace std;

PortNameParser::PortNameParser(const string& uri)
{
    string tmp = uri;
    int dotPos = tmp.find('.');
    int colonPos = tmp.find(':');

    if(colonPos != -1) 
    {
        port_ = tmp.substr(colonPos + 1);
        tmp = tmp.substr(0, colonPos);
    }

    fullName_ = tmp;

    if (dotPos != -1) 
    {
        instanceName_ = tmp.substr(dotPos + 1);
        tmp = tmp.substr(0, dotPos);
    }

    moduleName_ = tmp;
}


PortNameParser::~PortNameParser(void)
{
}


bool PortNameParser::operator== (const PortNameParser& other) const
{
    return (GetFullName() == other.GetFullName() &&
        GetInstanceName() == other.GetInstanceName() &&
        GetModuleName() == other.GetModuleName() /*&& GetPort() == other.GetPort()*/);
}


bool PortNameParser::operator!= (const PortNameParser& other) const
{
    return !(*this == other);
}


bool PortNameParser::operator < (const PortNameParser& other) const
{
    return (GetFullName() < other.GetFullName());
}


const string& PortNameParser::GetFullName(void) const
{
    return fullName_;
}


const string& PortNameParser::GetModuleName(void) const
{
    return moduleName_;
}


const string& PortNameParser::GetInstanceName(void) const
{
    return instanceName_;
}


const string& PortNameParser::GetPort(void) const
{
    return port_;
}
