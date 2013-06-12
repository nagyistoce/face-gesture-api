#pragma once

#include <string>
#include <vector>
#include "tbb/flow_graph.h"

#include "FaceDef.h"

namespace face 
{

//! Standalone executable algorithm
/*!
	\ingroup Core

	The abstract base class for algorithms that can be executed separately later.
	These algorithms are meant to be executable dynamically when needed - like
	generation of a database, or preprocess any inputs for latter real executions.
*/
class Algorithm
{
public:
	Algorithm(const std::string& name, const std::string& config);
	virtual ~Algorithm(void);

    virtual void operator() (tbb::flow::continue_msg) const = 0;

    const std::string& GetConfig(void) const;
    const std::string& GetName(void) const;

private:
    std::string name_;
	std::string config_;
};

}
