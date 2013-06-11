#pragma once

#include "Body.h"
#include "FaceDef.h"
#include "PortNameParser.h"


class ImageMessage;

class EmptyBody
:   public Body
{
public:
    enum InputPortNames
    {
        INPUT_IMAGE = 0
    };

    enum OutputPortNames
    {
        OUTPUT_DEFAULT = 0
    };

    EmptyBody(const VertexElement& vertexElement);
    EmptyBody(const EmptyBody& other);
    virtual ~EmptyBody(void);

    Body::OutputType operator() (Body::InputType1 input);
    void operator= (const EmptyBody& other);


private:
    virtual void Process(void);

    ImageMessage* imageWrapperIn_;
};

