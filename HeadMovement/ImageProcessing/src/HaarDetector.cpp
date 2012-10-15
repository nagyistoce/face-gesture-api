#include "HaarDetector.h"
#include "Definitions.h"

using namespace std;
using namespace cv;

HaarDetector::HaarDetector(const string& name)
:	name_(name),
	cascadeName_(FACE_CASCADE),
	scaleFactor_(1.2),
	minNeighbors_(2),
	flags_(0 | CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE | CV_HAAR_DO_ROUGH_SEARCH),
	minSize_(Size(100, 100)),
	maxSize_(Size()) 
{
	cout << "Loading cascade: " << cascadeName_ << endl;

	if(!cascade_.load(cascadeName_))
		CV_Error(1, "Could not load cascade classifier: " + cascadeName_ + ".");
}

HaarDetector::~HaarDetector(void)
{
}

void HaarDetector::SetFrame(const Mat& frame)
{
    frame_ = frame.clone();
}

void* HaarDetector::Run(void)
{
    if(!frame_.empty())
        Process();

    return reinterpret_cast<void*>(0);
}

void HaarDetector::Process(void)
{
	objects_.clear();

	Mat grayImage;
	Mat normalizedImage(cvRound(frame_.rows), cvRound(frame_.cols), CV_8UC1);

	cvtColor(frame_, grayImage, CV_BGR2GRAY);
	resize(grayImage, normalizedImage, normalizedImage.size());
	equalizeHist(normalizedImage, normalizedImage);

	cascade_.detectMultiScale(normalizedImage, objects_, scaleFactor_, minNeighbors_, flags_, minSize_, maxSize_);

	//return objects_;
}

const vector<Rect>& HaarDetector::GetObjects(void)
{
	return objects_;
}

const string& HaarDetector::GetName(void)
{
	return name_;
}
