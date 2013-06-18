#pragma once

#include <string>
#include <vector>

#include "FaceDef.h"
#include "Algorithm.h"


class TestAlgorithm 
:	public Algorithm
{
public:
    TestAlgorithm(const std::string& name, const std::string& config);
    virtual ~TestAlgorithm(void);

    void operator() (tbb::flow::continue_msg) const;
};
