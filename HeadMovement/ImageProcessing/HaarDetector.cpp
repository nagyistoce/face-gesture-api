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
	minSize_(Size(10, 10)),
	maxSize_(Size()) 
{
	cout << "Loading cascade: " << cascadeName_ << endl;

	if(!cascade_.load(cascadeName_))
		CV_Error(1, "Could not load cascade classifier: " + cascadeName_ + ".");
}

HaarDetector::~HaarDetector(void)
{
}

const vector<Rect>& HaarDetector::Detect(const Mat& image)
{
	objects_.clear();

	Mat grayImage;
	Mat normalizedImage(cvRound(image.rows), cvRound(image.cols), CV_8UC1);

	cvtColor(image, grayImage, CV_BGR2GRAY);
	resize(grayImage, normalizedImage, normalizedImage.size());
	equalizeHist(normalizedImage, normalizedImage);

#ifdef DEBUG_MODE
	double t = (double)cvGetTickCount();
#endif

	cascade_.detectMultiScale(normalizedImage, objects_, scaleFactor_, minNeighbors_, flags_, minSize_, maxSize_);

#ifdef DEBUG_MODE
	t = (double)cvGetTickCount() - t;
	printf("Detection time for %s = %g ms.\n", name_.c_str(), t / ((double)cvGetTickFrequency() * 1000.0));
#endif

	return objects_;
}

const vector<Rect>& HaarDetector::GetObjects(void)
{
	return objects_;
}

const string& HaarDetector::GetName(void)
{
	return name_;
}
