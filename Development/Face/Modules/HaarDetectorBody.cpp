#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

#include "HaarDetectorBody.h"
#include "Tracer.h"
#include "ImageMessage.h"
#include "RectangleMessage.h"
#include "GarbageCollector.h"
#include "GlobalSettings.h"
#include "HaarDetectorParam.h"

using namespace cv;
using namespace std;

namespace face 
{

HaarDetectorBody::HaarDetectorBody(const VertexElement& vertexElement)
:   Body(vertexElement),
    imageMessageIn_(NULL),
	prevImageMessageIn_(NULL),
	rectangleMessageIn_(NULL),
	param_(NULL)
{
	param_ = new HaarDetectorParam(GetModulConfigurationFs());

	if(!cascade_.load(GlobalSettingsConstPtr->GetDirectories().moduleSettings + GetModuleName() + "/" + param_->cascadeName))
	{
		CV_Error(-1, "Could not load cascade classifier: " + param_->cascadeName + ".");
	}
}


HaarDetectorBody::HaarDetectorBody(const HaarDetectorBody& other)
:   Body(other),
    imageMessageIn_(NULL),
	prevImageMessageIn_(NULL),
	rectangleMessageIn_(NULL),
	param_(NULL),
    cascade_(other.cascade_),
	objects_(other.objects_),
	prevObjects_(other.prevObjects_)
{
	if(other.param_)
		param_ = new HaarDetectorParam(*other.param_);

	if(other.imageMessageIn_)
		imageMessageIn_ = new ImageMessage(*other.imageMessageIn_);
	if(other.prevImageMessageIn_)
		prevImageMessageIn_ = new ImageMessage(*other.prevImageMessageIn_);
	if(other.rectangleMessageIn_)
		rectangleMessageIn_ = new RectangleMessage(*other.rectangleMessageIn_);
}


HaarDetectorBody::~HaarDetectorBody(void)
{
    delete imageMessageIn_;
	delete prevImageMessageIn_;
	delete rectangleMessageIn_;
	delete param_;
}


Body::OutputType HaarDetectorBody::operator() (Body::InputType2 input)
{
	imageMessageIn_ = dynamic_cast<ImageMessage*>(std::get<INPUT_IMAGE>(input));
	rectangleMessageIn_ = dynamic_cast<RectangleMessage*>(std::get<INPUT_RECTANGLE>(input));

	TRACE(GetFullName() + ": " + imageMessageIn_->GetMetaData().GetFrameNumber());

	BeforeProcess();
	Process();
	AfterProcess();

	GarbageCollectorPtr->InputHasBeenProcessed(rectangleMessageIn_, GarbageCollector::RELEASE_IF_PROCESSED);

	if(prevImageMessageIn_)
	{
		GarbageCollectorPtr->InputHasBeenProcessed(prevImageMessageIn_, GarbageCollector::NOTIFY_IF_PROCESSED);
	}

	prevImageMessageIn_ = imageMessageIn_;

    return output_;
}


void HaarDetectorBody::operator= (const HaarDetectorBody& other)
{
    WARNING("Equality operator has been disabled.");
    return;
}


void HaarDetectorBody::Process(void)
{
    const Mat& frameIn = imageMessageIn_->Normalized();

    outputFrame_ = imageMessageIn_->Rgb().clone();
	prevObjects_ = objects_;

	if(objects_.empty() || imageMessageIn_->GetMetaData().GetFrameNumber() % 30 == 0)
	{
		const vector<Rect>& rectangles = rectangleMessageIn_->GetRectangles();
		Rect firstRect(0, 0, frameIn.cols, frameIn.rows);

		if(rectangles.size() > 0)
            PartitionateFace(rectangles[0], &firstRect);

		Mat frameInRect = frameIn(firstRect);
		Mat normalizedRes(cvRound(frameInRect.rows * param_->imgScaleFactor), cvRound(frameInRect.cols * param_->imgScaleFactor), CV_8UC1);
		resize(frameInRect, normalizedRes, normalizedRes.size());

		objects_.clear();
		cascade_.detectMultiScale(
            normalizedRes, 
            objects_, 
            param_->scaleFactor, 
			param_->minNeighbors, 
            param_->flags, 
            param_->minSize, 
            param_->maxSize
		);

		if(!objects_.empty())
		{
			for(vector<Rect>::iterator r = objects_.begin(); r != objects_.end(); r++)
			{
				r->x = cvRound(r->x * param_->invImgScaleFactor) + firstRect.x;
				r->y = cvRound(r->y * param_->invImgScaleFactor) + firstRect.y;
				r->width = cvRound(r->width * param_->invImgScaleFactor);
				r->height = cvRound(r->height * param_->invImgScaleFactor);

				rectangle(outputFrame_, *r, Scalar(255.0, 0.0, 0.0), 2);
                putText(outputFrame_, GetFullName(), r->tl(), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255.0, 0.0, 0.0));
			}
		}
	}
	else if(!prevObjects_.empty() && prevImageMessageIn_)
	{
		const Mat& prevFrame = prevImageMessageIn_->Normalized();

		param_->ClearLKVectors();

		for(vector<Rect>::const_iterator r = prevObjects_.begin(); r != prevObjects_.end(); r++)
		{
			param_->prevPoints.push_back(Point(cvRound(r->x + r->width * 0.5), cvRound(r->y + r->height * 0.5)));
			param_->objSizes.push_back(r->size());
		}

		calcOpticalFlowPyrLK(
            prevFrame, frameIn, 
            param_->prevPoints, param_->nextPoints, 
            param_->status, param_->error, 
			param_->winSize, 
            param_->maxLevel, 
            param_->criteria, 
            param_->LKflags, 
            param_->minEigThreshold
		);

        const vector<Point2f>& actPoints = (param_->prevPoints.size() == param_->nextPoints.size() ? param_->nextPoints : param_->prevPoints);
        for(size_t i = 0; i < actPoints.size(); i++)
        {
            Point tl(cvRound(actPoints[i].x - param_->objSizes[i].width * 0.5), cvRound(actPoints[i].y - param_->objSizes[i].height * 0.5));
            Point br(cvRound(actPoints[i].x + param_->objSizes[i].width * 0.5), cvRound(actPoints[i].y + param_->objSizes[i].height * 0.5));
            objects_[i] = Rect(tl, br);

            rectangle(outputFrame_, objects_[i], Scalar(0.0, 0.0, 255.0), 2);
            putText(outputFrame_, GetFullName(), tl, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0.0, 0.0, 255.0));
        }
	}

    IMSHOW(GetFullName(), outputFrame_);

    output_ = HasSuccessor() ? new RectangleMessage(objects_) : NULL;
}


void HaarDetectorBody::PartitionateFace(const cv::Rect& fullRect, cv::Rect* partitionRect)
{
    if(GetInstanceName() == "leftEye")
    {
        partitionRect->x = cvRound(fullRect.x + fullRect.width * 0.5);
        partitionRect->y = cvRound(fullRect.y + fullRect.height * 0.1);
        partitionRect->width = cvRound(fullRect.width * 0.5);
        partitionRect->height = cvRound(fullRect.height * 0.5);
    }
    else if(GetInstanceName() == "rightEye")
    {
        partitionRect->x = fullRect.x;
        partitionRect->y = cvRound(fullRect.y + fullRect.height * 0.1);
        partitionRect->width = cvRound(fullRect.width * 0.5);
        partitionRect->height = cvRound(fullRect.height * 0.5);
    }
    else if(GetInstanceName() == "nose")
    {
        partitionRect->x = cvRound(fullRect.x + fullRect.width * 0.15);
        partitionRect->y = cvRound(fullRect.y + fullRect.height * 0.35);
        partitionRect->width = cvRound(fullRect.width * 0.60);
        partitionRect->height = cvRound(fullRect.height * 0.45);
    }
    else if(GetInstanceName() == "mouth")
    {
        partitionRect->x = cvRound(fullRect.x + fullRect.width * 0.10);
        partitionRect->y = cvRound(fullRect.y + fullRect.height * 0.65);
        partitionRect->width = cvRound(fullRect.width * 0.8);
        partitionRect->height = cvRound(fullRect.height * 0.3);
    }
    else
    {
        *partitionRect = fullRect;
    }
}

}
