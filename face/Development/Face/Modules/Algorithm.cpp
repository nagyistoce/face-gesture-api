#include "Algorithm.h"

namespace face 
{

Algorithm::Algorithm(const std::string& name, const std::string& config)
:   name_(name),	
    config_(config)
{
}


Algorithm::~Algorithm(void)
{
}


const std::string& Algorithm::GetName(void) const
{
    return name_;
}


const std::string& Algorithm::GetConfig(void) const
{
	return config_;
}

}
