#include "EmptyBody.h"
#include "Tracer.h"
#include "GarbageCollector.h"
#include "ImageWrapper.h"
#include "EmptyMessage.h"

using namespace cv;
using namespace std;


EmptyBody::EmptyBody(const VertexElement& vertexElement)
:   Body(vertexElement),
    imageWrapperIn_(NULL)
{
}


EmptyBody::EmptyBody(const EmptyBody& other)
:   Body(other),
    imageWrapperIn_(NULL)
{
    if(other.imageWrapperIn_)
        imageWrapperIn_ = new ImageWrapper(*other.imageWrapperIn_);
}


EmptyBody::~EmptyBody(void)
{
    delete imageWrapperIn_;
}


Body::OutputType EmptyBody::operator() (Body::InputType1 input)
{
    imageWrapperIn_ = dynamic_cast<ImageWrapper*>(input);

    BeforeProcess();
    Process();
    AfterProcess();

    GarbageCollectorPtr->InputHasBeenProcessed(input);

    return output_;
}


void EmptyBody::operator= (const EmptyBody& other)
{
    WARNING("Equality operator has been disabled.");
    return;
}


void EmptyBody::Process(void)
{
    if(HasSuccessor())
    {
        output_ = new EmptyMessage();
    }
}
