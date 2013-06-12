#include <windows.h>

#include "TestAlgorithm.h"
#include "Tracer.h"

namespace face 
{

TestAlgorithm::TestAlgorithm(const std::string& name, const std::string& config)
:	Algorithm(name, config)
{
    LOG("ctor(" + GetName() + ", " + GetConfig() + ")");
}


TestAlgorithm::~TestAlgorithm(void)
{
}


void TestAlgorithm::operator() (tbb::flow::continue_msg) const
{
    for(int i = 0; i < 15; i++)
    {
        LOG("Execute(" + GetName() + ", " + GetConfig() + "): " + i);
        
        if(GetConfig() == "general")
            Sleep(1000);
        else
            Sleep(2000);
    }
}

}
