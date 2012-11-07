#include "Definitions.h"

#include <ctime>
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
	procTime_(0.0),
    opticalFlow_(false)
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

void HaarDetector::SetFrame(const Mat& frame, const cv::Mat& prevFrame)
{
    frame_ = frame.clone();
    prevFrame_ = prevFrame.clone();
}

void* HaarDetector::Run(void)
{
	procTime_ = (double)cvGetTickCount();
    if(!frame_.empty())
	{
        Process();
		//Visualize();
	}
	procTime_ = (double)cvGetTickCount() - procTime_;

    return reinterpret_cast<void*>(0);
}

void HaarDetector::Process(void)
{
    vector<Rect> tmpObj = objects_;
    objects_.clear();
    opticalFlow_ = false;

    Mat grayFrame;
    Mat normalizedImage(cvRound(frame_.rows), cvRound(frame_.cols), CV_8UC1);

    cvtColor(frame_, grayFrame, CV_BGR2GRAY);
    resize(grayFrame, normalizedImage, normalizedImage.size());
    equalizeHist(normalizedImage, normalizedImage);

    cascade_.detectMultiScale(normalizedImage, objects_, scaleFactor_, minNeighbors_, flags_, minSize_, maxSize_);

    if(objects_.empty() && !tmpObj.empty() && !prevFrame_.empty())
    {
        objects_ = tmpObj;

        vector<Point2f> prevPoints, nextPoints;
        vector<Size> objSizes;
        vector<uchar> status;
        vector<float> err;
        TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);
        Size winSize(5, 5);

        Mat grayPrevFrame;
        cvtColor(prevFrame_, grayPrevFrame, CV_BGR2GRAY);

        for(vector<Rect>::const_iterator r = objects_.begin(); r != objects_.end(); r++)
        {
            Point center(cvRound(r->x + r->width * 0.5), cvRound(r->y + r->height * 0.5));
            prevPoints.push_back(center);
            objSizes.push_back(Size(r->width, r->height));
        }

        calcOpticalFlowPyrLK(grayPrevFrame, grayFrame, prevPoints, nextPoints, status, err, winSize, 3, termcrit, 0, 0.001);

        if(prevPoints.size() == nextPoints.size())
        {
            opticalFlow_ = true;
            for(size_t i = 0; i < nextPoints.size(); i++)
            {
                Point tl(nextPoints[i].x - objSizes[i].width * 0.5, nextPoints[i].y - objSizes[i].height * 0.5);
                Point br(nextPoints[i].x + objSizes[i].width * 0.5, nextPoints[i].y + objSizes[i].height * 0.5);
                objects_[i] = Rect(tl, br);
            }
        }
        else
        {
            objects_.clear();
        }
    }
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
    stringstream ss;

    ss << "Processing time of " << name_ << ": " << procTime_ / (cvGetTickFrequency() * 1000.0);
    VisualizerPtr->PutText(frame_, ss.str(), Point(10, 20));
    ss.str("");

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
        if(opticalFlow_)
            rectangle(frame_, Point(r->x, r->y), Point(r->x + r->width, r->y + r->height), colors[(i+1) % 8], 2);
        else
            rectangle(frame_, Point(r->x, r->y), Point(r->x + r->width, r->y + r->height), colors[i % 8], 2);
	}

	VisualizerPtr->ShowImage("HeadMovementAlgorithm - " + name_, frame_);
}
