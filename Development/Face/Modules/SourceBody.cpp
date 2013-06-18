#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "SourceBody.h"
#include "Tracer.h"
#include "ImageMessage.h"
#include "MetaData.h"
#include "GlobalSettings.h"
#include "SourceParam.h"

using namespace std;

namespace face 
{

SourceBody::SourceBody(const VertexElement& vertexElement)
:   Body(vertexElement),
	metaData_(NULL),
	param_(NULL),
	mediaEnd_(false)
{
	param_ = new SourceParam(GetModulConfigurationFs());
	metaData_ = new MetaData();

    if(GlobalSettingsPtr->GetValueAsString("live") == "true")
    {
        videoCapture_.open(param_->cameraId);
        if(videoCapture_.isOpened() == false)
		{
			stringstream ss;
			ss << param_->cameraId;
            CV_Error(-1, "Could not opened video capture for camera: " + ss.str());
		}
    }
    else
    {
		string fullPath = GlobalSettingsPtr->GetDirectories().videoInput + param_->videoFileName;
        videoCapture_.open(fullPath);
        if(videoCapture_.isOpened() == false)
            CV_Error(-1, "Could not opened video capture for: " + fullPath);
    }

	videoCapture_.set(CV_CAP_PROP_FRAME_WIDTH, param_->frameSize.width);
	videoCapture_.set(CV_CAP_PROP_FRAME_HEIGHT, param_->frameSize.height);
	videoCapture_.set(CV_CAP_PROP_FPS, param_->fps);
}


SourceBody::SourceBody(const SourceBody& other)
:   Body(other),
    videoCapture_(other.videoCapture_),
    metaData_(NULL),
	param_(NULL),
	mediaEnd_(other.mediaEnd_)
{
	if(other.metaData_)
		metaData_ = new MetaData(*other.metaData_);

	if(other.param_)
		param_ = new SourceParam(*other.param_);
}


SourceBody::~SourceBody(void)
{
	delete metaData_;
	delete param_;
}


bool SourceBody::operator() (Body::OutputType& output)
{
	BeforeProcess();
	Process();
	AfterProcess();

	output = output_;

	return !mediaEnd_;
}


void SourceBody::operator= (const SourceBody& other)
{
    WARNING("Equality operator has been disabled.");
	return;
}


void SourceBody::Process(void)
{
	videoCapture_ >> outputFrame_;

	if(outputFrame_.empty())
	{
		mediaEnd_ = true;
		return;
	}

	metaData_->SetTimestamp(GetTimestamp());
	resize(outputFrame_, outputFrame_, param_->frameSize);

	TRACE(GetFullName() + ": " + metaData_->GetFrameNumber());
	IMSHOW(GetFullName(), outputFrame_);

	if(HasSuccessor())
	{
		output_ = new ImageMessage(outputFrame_, *metaData_);
	}

	metaData_->IncrementFrameNumber();
}

}
