#pragma once

#include <string>
#include <utility>
#include <vector>
#include <list>

class PortNameParser
{
public:
    PortNameParser(const std::string& uri);
    ~PortNameParser(void);

    bool operator== (const PortNameParser& other) const;
    bool operator!= (const PortNameParser& other) const;
    bool operator < (const PortNameParser& other) const;

    const std::string& GetFullName(void) const;
    const std::string& GetModuleName(void) const;
    const std::string& GetInstanceName(void) const;
    const std::string& GetPort(void) const;


private:
    std::string fullName_;
    std::string moduleName_;
    std::string instanceName_;
    std::string port_;
};

typedef std::list<PortNameParser> ChildrenList;
typedef std::pair<PortNameParser, ChildrenList> VertexElement;
typedef std::vector<VertexElement> VertexContainer;
