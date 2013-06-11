#include "EmptyBody.h"
#include "Tracer.h"
#include "GarbageCollector.h"
#include "ImageMessage.h"
#include "EmptyMessage.h"
#include "RectangleMessage.h"

using namespace cv;
using namespace std;


EmptyBody::EmptyBody(const VertexElement& vertexElement)
:   Body(vertexElement),
    imageMessageIn_(NULL)
{
}


EmptyBody::EmptyBody(const EmptyBody& other)
:   Body(other),
    imageMessageIn_(NULL)
{
    if(other.imageMessageIn_)
        imageMessageIn_ = new ImageMessage(*other.imageMessageIn_);
}


EmptyBody::~EmptyBody(void)
{
    delete imageMessageIn_;
}


Body::OutputType EmptyBody::operator() (Body::InputType1 input)
{
    imageMessageIn_ = dynamic_cast<ImageMessage*>(input);

    BeforeProcess();
    Process();
    AfterProcess();

    GarbageCollectorPtr->InputHasBeenProcessed(input, GarbageCollector::NOTIFY_IF_PROCESSED);

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
		if(GetInstanceName() == "face")
		{
			std::vector<cv::Rect> rectangles;
			rectangles.push_back(cv::Rect(0, 0, imageMessageIn_->GetWidth(), imageMessageIn_->GetHeight()));
			output_ = new RectangleMessage(rectangles);
		}
		else
		{
			output_ = new EmptyMessage();
		}
    }
}
