#pragma once

#include "Body.h"
#include "FaceDef.h"
#include "PortNameParser.h"
#include "Node.h"


class SourceLimiterBody
:   public Body
{
public:
    enum InputPortNames
    {
        INPUT_IMAGE = 0
    };

    enum OutputPortNames
    {
        OUTPUT_DEFAULT = 0,
        OUTPUT_LIMITER
    };

    SourceLimiterBody(const VertexElement& vertexElement);
    SourceLimiterBody(const SourceLimiterBody& other);
    virtual ~SourceLimiterBody(void);

    void operator() (const Body::InputType1& input, Node::MultiNodeContinueType::output_ports_type& output);
    void operator= (const SourceLimiterBody& other);

private:
    virtual void Process(void);
};
