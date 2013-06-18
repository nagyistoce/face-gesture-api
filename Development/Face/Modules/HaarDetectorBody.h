#pragma once

#include <opencv2/objdetect/objdetect.hpp>
#include "Body.h"
#include "FaceDef.h"
#include "PortNameParser.h"

namespace face 
{

class ImageMessage;
class RectangleMessage;
class HaarDetectorParam;

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

    Body::OutputType operator() (Body::InputType2 input);
    void operator= (const HaarDetectorBody& other);


private:
    virtual void Process(void);

    ImageMessage* imageMessageIn_;
	ImageMessage* prevImageMessageIn_;
	RectangleMessage* rectangleMessageIn_;
	HaarDetectorParam* param_;

    cv::CascadeClassifier cascade_;
	std::vector<cv::Rect> prevObjects_, objects_;
};

}
