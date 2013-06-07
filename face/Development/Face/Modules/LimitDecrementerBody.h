#pragma once

#include "Body.h"
#include "FaceDef.h"
#include "PortNameParser.h"
#include "Node.h"


class LimitDecrementerBody
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

    LimitDecrementerBody(const VertexElement& vertexElement);
    LimitDecrementerBody(const LimitDecrementerBody& other);
    virtual ~LimitDecrementerBody(void);

    void operator() (const Body::InputType1& input, Node::MultiNodeContinueType::output_ports_type& output);
    void operator= (const LimitDecrementerBody& other);

private:
    virtual void Process(void);
};
