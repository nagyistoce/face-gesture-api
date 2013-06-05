#pragma once

#include "Body.h"
#include "FaceDef.h"
#include "PortNameParser.h"


class ImageWrapper;

class HaarDetectorBody
:   public Body
{
public:
    enum InputPortNames
    {
        INPUT_IMAGE = 0,
        INPUT_RECTANGLE
    };

    enum OutputPortNames
    {
        OUTPUT_DEFAULT = 0
    };

    HaarDetectorBody(const VertexElement& vertexElement);
    HaarDetectorBody(const HaarDetectorBody& other);
    virtual ~HaarDetectorBody(void);

    Body::OutputType operator() (Body::InputType1 input);
    void operator= (const HaarDetectorBody& other);


private:
    virtual void Process(void);

    ImageWrapper* imageWrapperIn_;
};
