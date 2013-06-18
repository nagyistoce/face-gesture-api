#include "SourceBody.h"
#include "Tracer.h"
#include "ImageMessage.h"
#include "MetaData.h"

using namespace std;


SourceBody::SourceBody(const VertexElement& vertexElement)
:   Body(vertexElement),
	metaData_(NULL),
	sourceType_(SourceBody::CAMERA),
	cameraId_(0),
	videoFilePath_("")
{
	if(instanceName_ == "front")
		metaData_ = new MetaData(MetaData::POSITION_FRONT, 0, 0);
	else
		metaData_ = new MetaData(MetaData::POSITION_UNDEFINED, 0, 0);

    if(sourceType_ == SourceBody::CAMERA)
    {
        videoCapture_.open(cameraId_);
        if(videoCapture_.isOpened() == false)
            CV_Error(-1, "Could not opened video capture for camera: " + cameraId_);
    }
    else
    {
        videoCapture_.open(videoFilePath_);
        if(videoCapture_.isOpened() == false)
            CV_Error(-1, "Could not opened video capture for: " + videoFilePath_);
    }
}


SourceBody::SourceBody(const SourceBody& other)
:   Body(other),
	sourceType_(other.sourceType_),
	cameraId_(other.cameraId_),
	videoFilePath_(other.videoFilePath_),
    videoCapture_(other.videoCapture_),
    metaData_(NULL)
{
    if(other.metaData_)
	    metaData_ = new MetaData(*other.metaData_);
}


SourceBody::~SourceBody(void)
{
	delete metaData_;
}


bool SourceBody::operator() (Body::OutputType& output)
{
	BeforeProcess();
	Process();
	AfterProcess();

	output = output_;

	return true;
}


void SourceBody::operator= (const SourceBody& other)
{
    WARNING("Equality operator has been disabled.");
	return;
}


void SourceBody::Process(void)
{
	metaData_->SetTimestamp(GetTimestamp());

	videoCapture_ >> outputFrame_;

	TRACE(GetFullName() + ": " + metaData_->GetFrameNumber());
	IMSHOW(GetFullName(), outputFrame_);

	if(HasSuccessor())
	{
		output_ = new ImageMessage(outputFrame_, *metaData_);
	}

	metaData_->IncrementFrameNumber();
}
