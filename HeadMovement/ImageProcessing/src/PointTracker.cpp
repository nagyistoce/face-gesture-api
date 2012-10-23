#include "Definitions.h"
#include "PointTracker.h"

#include "Visualizer.h"
#include "LocalSettings.h"

using namespace std;
using namespace cv;

PointTracker::PointTracker(string name)
:   name_(name),
    pyrScale_(0.5),
    levels_(3),
    winsize_(15),
    iterations_(3),
    polyN_(5),
    polySigma_(1.2),
    flags_(0),
	procTime_(0.0)
{
    LoadSettingsFromFileStorage();
}


PointTracker::~PointTracker(void)
{
}

void PointTracker::LoadSettingsFromFileStorage(void)
{
    string fileName = LocalSettingsPtr->GetMotionDirectory() + "Settings." + name_ + ".xml";
    FileStorage fileStorage(fileName, FileStorage::READ, "UTF-8");

    if(!fileStorage.isOpened())
        CV_Error(1, "Setting XML does not exist for " + name_ + "!");

    fileStorage["pyrScale"] >> pyrScale_;
    fileStorage["levels"] >> levels_;
    fileStorage["winsize"] >> winsize_;
    fileStorage["iterations"] >> iterations_;
    fileStorage["polyN"] >> polyN_;
    fileStorage["polySigma"] >> polySigma_;
    fileStorage["flags"] >> flags_;
}

void PointTracker::Process(const Mat& frame, const Mat& prevFrame, const Rect& rect, const vector<KeyPoint>& keyPoints)
{
	CV_Assert(!frame.empty());
	CV_Assert(!prevFrame.empty());

	//Point2f center(rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f);
	//double radius = (rect.width + rect.height) * 0.25f;
	procTime_ = (double)cvGetTickCount();

	cvtColor(frame, grayFrame_, CV_BGR2GRAY);
	cvtColor(prevFrame, grayPrevFrame_, CV_BGR2GRAY);

	calcOpticalFlowFarneback(grayPrevFrame_, grayFrame_, flow_, pyrScale_, levels_, winsize_, iterations_, polyN_, polySigma_, flags_);


	//for(size_t i = 0; i < keyPoints.size(); i++)
	//{
	//	double dst = sqrt(pow(center.x - keyPoints[i].pt.x, 2) + pow(center.y - keyPoints[i].pt.y, 2));

	//	if(dst < radius)
	//		innerPoints.push_back(points[i]);
	//}

	procTime_ = (double)cvGetTickCount() - procTime_;

	Visualize();
}

string PointTracker::GetName(void) const
{
    return name_;
}

void PointTracker::Visualize(void)
{
	stringstream ss;
	Mat flowMap;

	cvtColor(grayPrevFrame_, flowMap, CV_GRAY2BGR);

	for(int y = 0; y < flowMap.rows; y += 10)
		for(int x = 0; x < flowMap.cols; x += 10)
		{
			const Point2f& fxy = flow_.at<Point2f>(y, x);
			line(flowMap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), Scalar(0, 255, 0));
			circle(flowMap, Point(x, y), 2, Scalar(0, 255, 0), -1);
		}

	ss << "Processing time of MotionHistory: " << procTime_ / (cvGetTickFrequency() * 1000.0);
	VisualizerPtr->PutText(flowMap, ss.str(), Point(10, 20));
	ss.str("");

	VisualizerPtr->ShowImage("FlowMap", flowMap);
}
