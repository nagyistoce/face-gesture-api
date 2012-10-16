#include "Definitions.h"

#include "HaarDetector.h"
#include "LocalSettings.h"
#include "Visualizer.h"

using namespace std;
using namespace cv;

HaarDetector::HaarDetector(const string& name)
:	name_(name),
	cascadeName_("haarcascade_frontalface_alt.xml"),
	scaleFactor_(1.2),
	minNeighbors_(2),
	flags_(0),
	minSize_(Size()),
	maxSize_(Size()),
	procTime_(0.0)
{
	LoadSettingsFromFileStorage();

	//namedWindow("HeadMovementAlgorithm - " + name_, WINDOW_AUTOSIZE);
}

HaarDetector::~HaarDetector(void)
{
}

void HaarDetector::LoadSettingsFromFileStorage(void)
{
	string fileName = LocalSettingsPtr->GetDetectorDirectory() + "Settings." + name_ + ".xml";
	FileStorage fileStorage(fileName, FileStorage::READ, "UTF-8");

	if(!fileStorage.isOpened())
		CV_Error(1, "Setting XML does not exist for " + name_ + "!");

	fileStorage["cascadeName"] >> cascadeName_;

	cout << "Loading cascade: " << cascadeName_ << endl;
	if(!cascade_.load(LocalSettingsPtr->GetDetectorDirectory() + cascadeName_))
		CV_Error(1, "Could not load cascade classifier: " + cascadeName_ + ".");

	fileStorage["scaleFactor"] >> scaleFactor_;
	fileStorage["minNeighbors"] >> minNeighbors_;
	fileStorage["flags"] >> flags_;

	if(fileStorage["minWidth"].isNone() == false && fileStorage["minHeight"].isNone() == false)
	{
		fileStorage["minWidth"] >> minSize_.width;
		fileStorage["minHeight"] >> minSize_.height;
	}
	
	if(fileStorage["maxWidth"].isNone() == false && fileStorage["maxHeight"].isNone() == false)
	{
		fileStorage["maxWidth"] >> maxSize_.width;
		fileStorage["maxHeight"] >> maxSize_.height;
	}
}

void HaarDetector::SetFrame(const Mat& frame)
{
    frame_ = frame.clone();
}

void* HaarDetector::Run(void)
{
	procTime_ = (double)cvGetTickCount();
    if(!frame_.empty())
	{
        Process();
		Visualize();
	}
	procTime_ = (double)cvGetTickCount() - procTime_;

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
}

const vector<Rect>& HaarDetector::GetObjects(void)
{
	return objects_;
}

const string& HaarDetector::GetName(void)
{
	return name_;
}

void HaarDetector::Visualize(void)
{
	char buffer[500];

	sprintf_s(buffer, 500, "Processing time of %s: %.2lf ms.", name_.c_str(), procTime_ / (cvGetTickFrequency() * 1000.0));
	VisualizerPtr->PutText(frame_, buffer, Point(10, 20));

	int i = 0;
	const static Scalar colors[8] =  { 
		CV_RGB(0 ,0, 255),
		CV_RGB(0, 128, 255),
		CV_RGB(0, 255, 255),
		CV_RGB(0, 255, 0),
		CV_RGB(255, 128, 0),
		CV_RGB(255, 255, 0),
		CV_RGB(255, 0, 0),
		CV_RGB(255, 0, 255)
	};

	for(vector<Rect>::const_iterator r = objects_.begin(); r != objects_.end(); r++, i++)
	{
		Point center(cvRound(r->x + r->width * 0.5), cvRound(r->y + r->height * 0.5));
		int radius = cvRound((r->width + r->height) * 0.25);
		circle(frame_, center, radius, colors[i % 8], 3);
	}

	VisualizerPtr->ShowImage("HeadMovementAlgorithm - " + name_, frame_);
}
