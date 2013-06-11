#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

#include "HaarDetectorBody.h"
#include "Tracer.h"
#include "ImageMessage.h"
#include "RectangleMessage.h"
#include "GarbageCollector.h"
#include "GlobalSettings.h"

using namespace cv;
using namespace std;


HaarDetectorBody::HaarDetectorBody(const VertexElement& vertexElement)
:   Body(vertexElement),
    imageWrapperIn_(NULL),
	prevImageMessageIn_(NULL),
    cascadeName_(""),
    scaleFactor_(1.2),
	imgScaleFactor_(1.0),
	invImgScaleFactor_(1.0),
    minNeighbors_(2),
    flags_(0),
    minSize_(Size()),
    maxSize_(Size())
{
    const cv::FileStorage& configurationFs = GetModulConfigurationFs();

    if(!configurationFs["cascadeName"].empty() && configurationFs["cascadeName"].isString())
	{
        configurationFs["cascadeName"] >> cascadeName_;
	}

	if(!cascade_.load(GlobalSettingsConstPtr->GetDirectories().moduleSettings + GetModuleName() + "/" + cascadeName_))
	{
		CV_Error(-1, "Could not load cascade classifier: " + cascadeName_ + ".");
	}

	if(!configurationFs["scaleFactor"].empty() && configurationFs["scaleFactor"].isReal())
	{
		configurationFs["scaleFactor"] >> scaleFactor_;
	}

	if(!configurationFs["imgScaleFactor"].empty() && configurationFs["imgScaleFactor"].isReal())
	{
		configurationFs["imgScaleFactor"] >> imgScaleFactor_;

		if(imgScaleFactor_ >= 1.0)
			imgScaleFactor_ = 1.0;
		else if(imgScaleFactor_ <= 0.0)
			imgScaleFactor_ = 0.1;

		invImgScaleFactor_ = 1.0 / imgScaleFactor_;
	}

    if(!configurationFs["minNeighbors"].empty() && configurationFs["minNeighbors"].isInt())
	{
        configurationFs["minNeighbors"] >> minNeighbors_;
	}

    if(!configurationFs["flags"].empty() && configurationFs["flags"].isInt())
	{
        configurationFs["flags"] >> flags_;
	}

    if(!configurationFs["minWidth"].empty() && !configurationFs["minHeight"].empty() && 
		configurationFs["minWidth"].isInt() && configurationFs["minHeight"].isInt())
    {
        int minWidth, minHeight;
        configurationFs["minWidth"] >> minWidth;
        configurationFs["minHeight"] >> minHeight;
        minSize_ = Size(minWidth, minHeight);
    }

    if(!configurationFs["maxWidth"].empty() && !configurationFs["maxHeight"].empty() && 
		configurationFs["maxWidth"].isInt() && configurationFs["maxHeight"].isInt())
    {
        int maxWidth, maxHeight;
        configurationFs["maxWidth"] >> maxWidth;
        configurationFs["maxHeight"] >> maxHeight;
        maxSize_ = Size(maxWidth, maxHeight);
    }
}


HaarDetectorBody::HaarDetectorBody(const HaarDetectorBody& other)
:   Body(other),
    imageWrapperIn_(NULL),
	prevImageMessageIn_(NULL),
    cascade_(other.cascade_),
    cascadeName_(other.cascadeName_),
    scaleFactor_(other.scaleFactor_),
	imgScaleFactor_(other.imgScaleFactor_),
	invImgScaleFactor_(other.invImgScaleFactor_),
    minNeighbors_(other.minNeighbors_),
    flags_(other.flags_),
    minSize_(other.minSize_),
    maxSize_(other.maxSize_),
	objects_(other.objects_)
{
	if(other.imageWrapperIn_)
		imageWrapperIn_ = new ImageMessage(*other.imageWrapperIn_);

	if(other.prevImageMessageIn_)
		prevImageMessageIn_ = new ImageMessage(*other.prevImageMessageIn_);

	other.grayFrame_.copyTo(grayFrame_);
	other.normalizedImage_.copyTo(normalizedImage_);
}


HaarDetectorBody::~HaarDetectorBody(void)
{
    delete imageWrapperIn_;
	delete prevImageMessageIn_;
}


Body::OutputType HaarDetectorBody::operator() (Body::InputType2 input)
{
	imageWrapperIn_ = dynamic_cast<ImageMessage*>(std::get<INPUT_IMAGE>(input));

	TRACE(GetFullName() + ": " + imageWrapperIn_->GetMetaData().GetFrameNumber());

	BeforeProcess();
	Process();
	AfterProcess();

	if(prevImageMessageIn_)
		GarbageCollectorPtr->InputHasBeenProcessed(prevImageMessageIn_, GarbageCollector::NOTIFY_IF_PROCESSED);

	prevImageMessageIn_ = imageWrapperIn_;

    return output_;
}


void HaarDetectorBody::operator= (const HaarDetectorBody& other)
{
    WARNING("Equality operator has been disabled.");
    return;
}


void HaarDetectorBody::Process(void)
{
    const Mat& frameIn = imageWrapperIn_->Rgb();
	const Mat& prevFrame = prevImageMessageIn_->Rgb();

    outputFrame_ = frameIn.clone();

	Mat grayFrameRes(cvRound(frameIn.rows * imgScaleFactor_), cvRound(frameIn.cols * imgScaleFactor_), CV_8UC1);
    cvtColor(frameIn, grayFrame_, CV_BGR2GRAY);
	resize(grayFrame_, grayFrameRes, grayFrameRes.size());
    equalizeHist(grayFrameRes, normalizedImage_);

	vector<Rect> prevObjects = objects_;

	if(objects_.empty() || imageWrapperIn_->GetMetaData().GetFrameNumber() % 30 == 0)
	{
		objects_.clear();
		cascade_.detectMultiScale(normalizedImage_, objects_, scaleFactor_, minNeighbors_, flags_, minSize_, maxSize_);

		if(!objects_.empty())
		{
			for(vector<Rect>::iterator r = objects_.begin(); r != objects_.end(); r++)
			{
				r->x = cvRound(r->x * invImgScaleFactor_);
				r->y = cvRound(r->y * invImgScaleFactor_);
				r->width = cvRound(r->width * invImgScaleFactor_);
				r->height = cvRound(r->height * invImgScaleFactor_);

				rectangle(outputFrame_, Point(r->x, r->y), Point(r->x + r->width, r->y + r->height), Scalar(255, 0, 0), 2);
			}
		}
	}

	else if(/*objects_.empty() &&*/ !prevObjects.empty() && !prevFrame.empty())
	{
		//objects_ = prevObjects;

		vector<Point2f> prevPoints, nextPoints;
		vector<Size> objSizes;
		vector<uchar> status;
		vector<float> err;
		TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);
		Size winSize(5, 5);

		Mat grayPrevFrame;
		cvtColor(prevFrame, grayPrevFrame, CV_BGR2GRAY);

		for(vector<Rect>::const_iterator r = prevObjects.begin(); r != prevObjects.end(); r++)
		{
			Point center(cvRound(r->x + r->width * 0.5), cvRound(r->y + r->height * 0.5));
			prevPoints.push_back(center);
			objSizes.push_back(Size(r->width, r->height));
		}

		calcOpticalFlowPyrLK(grayPrevFrame, grayFrame_, prevPoints, nextPoints, status, err, winSize, 3, termcrit, 0, 0.001);

		if(prevPoints.size() == nextPoints.size())
		{
			for(size_t i = 0; i < nextPoints.size(); i++)
			{
				Point tl(cvRound(nextPoints[i].x - objSizes[i].width * 0.5), cvRound(nextPoints[i].y - objSizes[i].height * 0.5));
				Point br(cvRound(nextPoints[i].x + objSizes[i].width * 0.5), cvRound(nextPoints[i].y + objSizes[i].height * 0.5));
				objects_[i] = Rect(tl, br);
			}
		}
		else
		{
			objects_.clear();
		}

		for(vector<Rect>::iterator r = objects_.begin(); r != objects_.end(); r++)
		{
			rectangle(outputFrame_, Point(r->x, r->y), Point(r->x + r->width, r->y + r->height), Scalar(0, 0, 255), 2);
		}
	}

    IMSHOW(GetFullName(), outputFrame_);

    output_ = HasSuccessor() ? new RectangleMessage(objects_) : NULL;
}
