#include "HaarDetectorBody.h"
#include "Tracer.h"
#include "ImageWrapper.h"
#include "MetaData.h"

using namespace std;


HaarDetectorBody::HaarDetectorBody(const VertexElement& vertexElement)
:   Body(vertexElement),
    imageWrapperIn_(NULL)
{
}


HaarDetectorBody::HaarDetectorBody(const HaarDetectorBody& other)
:   Body(other),
    imageWrapperIn_(NULL)
{
    if(other.imageWrapperIn_)
        imageWrapperIn_ = new ImageWrapper(*other.imageWrapperIn_);
}


HaarDetectorBody::~HaarDetectorBody(void)
{
    delete imageWrapperIn_;
}


Body::OutputType HaarDetectorBody::operator() (Body::InputType1 input)
{
    if(HasSuccessor() == false)
        return NULL;

    imageWrapperIn_ = dynamic_cast<ImageWrapper*>(input);
    output_ = new ImageWrapper(*imageWrapperIn_); 

    return output_;
}


void HaarDetectorBody::operator= (const HaarDetectorBody& other)
{
    WARNING("Equality operator has been disabled.");
    return;
}


void HaarDetectorBody::Process(void)
{
    TRACE("HaarDetectorBody: " + imageWrapperIn_->GetMetaData().GetFrameNumber());
    //IMSHOW(GetFullName(), outputFrame_);
}
