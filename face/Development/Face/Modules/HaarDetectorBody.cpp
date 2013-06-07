#include <opencv2/imgproc/imgproc.hpp>

#include "HaarDetectorBody.h"
#include "Tracer.h"
#include "ImageWrapper.h"
#include "MetaData.h"
#include "GarbageCollector.h"
#include "GlobalSettings.h"

using namespace cv;
using namespace std;


HaarDetectorBody::HaarDetectorBody(const VertexElement& vertexElement)
:   Body(vertexElement),
    imageWrapperIn_(NULL),
    cascadeName_(""),
    scaleFactor_(1.2),
    minNeighbors_(2),
    flags_(0),
    minSize_(Size()),
    maxSize_(Size())
{
    const cv::FileStorage& configurationFs = GetModulConfigurationFs();

    if(!configurationFs["cascadeName"].empty() && configurationFs["cascadeName"].isString())
        configurationFs["cascadeName"] >> cascadeName_;

    if(!configurationFs["scaleFactor"].empty() && configurationFs["scaleFactor"].isReal())
        configurationFs["scaleFactor"] >> scaleFactor_;

    if(!configurationFs["minNeighbors"].empty() && configurationFs["minNeighbors"].isInt())
        configurationFs["minNeighbors"] >> minNeighbors_;

    if(!configurationFs["flags"].empty() && configurationFs["flags"].isInt())
        configurationFs["flags"] >> flags_;

    if(!configurationFs["minWidth"].empty() && !configurationFs["minHeight"].empty() && configurationFs["minWidth"].isInt() && configurationFs["minHeight"].isInt())
    {
        int minWidth, minHeight;
        configurationFs["minWidth"] >> minWidth;
        configurationFs["minHeight"] >> minHeight;
        minSize_ = Size(minWidth, minHeight);
    }

    if(!configurationFs["maxWidth"].empty() && !configurationFs["maxHeight"].empty() && configurationFs["maxWidth"].isInt() && configurationFs["maxHeight"].isInt())
    {
        int maxWidth, maxHeight;
        configurationFs["maxWidth"] >> maxWidth;
        configurationFs["maxHeight"] >> maxHeight;
        maxSize_ = Size(maxWidth, maxHeight);
    }

    if(!cascade_.load(GlobalSettingsConstPtr->GetDirectories().moduleSettings + GetModuleName() + "/" + cascadeName_))
        CV_Error(-1, "Could not load cascade classifier: " + cascadeName_ + ".");
}


HaarDetectorBody::HaarDetectorBody(const HaarDetectorBody& other)
:   Body(other),
    imageWrapperIn_(NULL),
    cascade_(other.cascade_),
    cascadeName_(other.cascadeName_),
    scaleFactor_(other.scaleFactor_),
    minNeighbors_(other.minNeighbors_),
    flags_(other.flags_),
    minSize_(other.minSize_),
    maxSize_(other.maxSize_)
{
    if(other.imageWrapperIn_)
        imageWrapperIn_ = new ImageWrapper(*other.imageWrapperIn_);
}


HaarDetectorBody::~HaarDetectorBody(void)
{
    delete imageWrapperIn_;
}


Body::OutputType HaarDetectorBody::operator() (Body::InputType2 input)
{
	imageWrapperIn_ = dynamic_cast<ImageWrapper*>(std::get<INPUT_IMAGE>(input));

	BeforeProcess();
	Process();
	AfterProcess();

	GarbageCollectorPtr->InputHasBeenProcessed(std::get<INPUT_IMAGE>(input));

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
    Mat outputRgb = frameIn.clone();

    TRACE(GetFullName() + ": " + imageWrapperIn_->GetMetaData().GetFrameNumber());

    Mat grayFrame, normalizedImage;

    cvtColor(frameIn, grayFrame, CV_BGR2GRAY);
    equalizeHist(grayFrame, normalizedImage);

    std::vector<cv::Rect> objects;
    cascade_.detectMultiScale(normalizedImage, objects, scaleFactor_, minNeighbors_, flags_, minSize_, maxSize_);

    if(!objects.empty())
    {
        Rect r = objects[0];
        rectangle(outputRgb, Point(r.x, r.y), Point(r.x + r.width, r.y + r.height), Scalar(255, 0, 0), 2);
    }

    IMSHOW(GetFullName(), outputRgb);
	output_ = /*HasSuccessor() ? new ImageWrapper(*imageWrapperIn_) :*/ NULL;
}
