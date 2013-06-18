#pragma once

#include <opencv2/objdetect/objdetect.hpp>
#include "Body.h"
#include "FaceDef.h"
#include "PortNameParser.h"


class ImageMessage;

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

    ImageMessage* imageWrapperIn_;
	ImageMessage* prevImageMessageIn_;

    cv::CascadeClassifier cascade_;

    std::string cascadeName_;
    double scaleFactor_;
	double imgScaleFactor_;
	double invImgScaleFactor_;
    int minNeighbors_;
    int flags_;
    cv::Size minSize_;
    cv::Size maxSize_;

	cv::Mat grayFrame_; 
	cv::Mat normalizedImage_;

	std::vector<cv::Rect> objects_;
};
