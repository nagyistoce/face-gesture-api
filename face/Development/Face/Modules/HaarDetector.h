#pragma once

#include <tbb/flow_graph.h>

#include "Module.h"
#include "FaceDef.h"
#include "PortNameParser.h"
#include "TbbNode.h"


class ImageWrapper;

class HaarDetector
:	public Module
{
public:
    //typedef Message* InputType;
    //typedef tbb::flow::function_node<InputType, Module::OutputType> FunctionNodeType;

    enum PortNames
    {
        INPUT_IMAGE = 0,
        INPUT_RECTANGLE,
        OUTPUT_DEFAULT
    };

    HaarDetector(const ConnectionElement& connectionElement);
    HaarDetector(const HaarDetector& other);
    virtual ~HaarDetector(void);
    virtual void CreateConnection(PredecessorMap& predecessorMap);

    TbbNode::OutputType operator() (TbbNode::InputType1 input);
    HaarDetector& operator=(const HaarDetector& /*other*/) {
        return *this;
    }
    //FunctionNodeType* GetNode(void);


private:
    virtual void DefinePorts(void);
    virtual void Process(void);

    //FunctionNodeType* functionNode_;
    ImageWrapper* imageWrapperIn_;
};
