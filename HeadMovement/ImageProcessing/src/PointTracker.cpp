#include <cmath>

#include "Definitions.h"
#include "PointTracker.h"

#include "Visualizer.h"
#include "LocalSettings.h"

using namespace std;
using namespace cv;

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

PointTracker::PointTracker(string name)
:   name_(name),
    pyrScale_(0.5),
    levels_(3),
    winsize_(15),
    iterations_(3),
    polyN_(5),
    polySigma_(1.2),
    flags_(0),
	procTime_(0.0),
	showBgrFlow_(0),
	angle_(0.0),
	direction_(make_pair(Point2f(0.0f, 0.0f), Point2f(0.0f, 0.0f)))
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
	fileStorage["showBgrFlow"] >> showBgrFlow_;
}

void PointTracker::Process(const Mat& frame, const Mat& prevFrame, const Rect& rect, const vector<KeyPoint>& keyPoints)
{
	CV_Assert(!frame.empty());
	CV_Assert(!prevFrame.empty());

	procTime_ = (double)cvGetTickCount();

	cvtColor(frame, grayFrame_, CV_BGR2GRAY);
	cvtColor(prevFrame, grayPrevFrame_, CV_BGR2GRAY);
    //grayFrame_ = grayFrame_(rect);
    //grayPrevFrame_ = grayPrevFrame_(rect);

	calcOpticalFlowFarneback(grayPrevFrame_, grayFrame_, flow_, pyrScale_, levels_, winsize_, iterations_, polyN_, polySigma_, flags_);

	Point2f center(rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f);
	double radius = (rect.width + rect.height) * 0.25f;
	keyPointMask_ = Mat::zeros(grayFrame_.size(), CV_8UC1);

	for(size_t i = 0; i < keyPoints.size(); i++)
	{
		double dst = sqrt(pow(keyPoints[i].pt.x - center.x, 2) + pow(keyPoints[i].pt.y - center.y, 2));

		if(dst < radius)
			circle(keyPointMask_, Point(keyPoints[i].pt), cvRound(keyPoints[i].size / 2.0), Scalar(255), -1);
	}

	Point2f sumStartPt(0.0f, 0.0f);
	Point2f sumEndPt(0.0f, 0.0f);
	int count = 0;

	for(int y = 0; y < flow_.rows; y++)
	{
		for(int x = 0; x < flow_.cols; x++)
		{
			if(keyPointMask_.at<uchar>(y, x) == 255)
			{
				const Point2f& fxy = flow_.at<Point2f>(y, x);

				sumStartPt += Point2f(float(x), float(y));
				sumEndPt += Point2f(x + fxy.x, y + fxy.y);
				count++;
			}
		}
	}

	direction_ = make_pair(Point2f(0.0f, 0.0f), Point2f(0.0f, 0.0f));
	if(count != 0)
	{
		direction_.first = Point2f(sumStartPt.x / float(count), sumStartPt.y / float(count));
		direction_.second = Point2f(sumEndPt.x / float(count), sumEndPt.y / float(count));
	}
	angle_ = Bearing(direction_.first, direction_.second);

	procTime_ = (double)cvGetTickCount() - procTime_;

	Visualize();
}


double PointTracker::Bearing(Point2f start, Point2f end)
{
	// x and y args to atan2() swapped to rotate resulting angle 90 degrees
	// (Thus angle in respect to +Y axis instead of +X axis)
	double radian = atan2(start.x - end.x, end.y - start.y);
	double angle = (radian * 180.0) / M_PI;

	// Ensure result is in interval [0, 360)
	// Subtract because positive degree angles go clockwise
	return cvRound(360.0 - angle) %  360;
}

const string& PointTracker::GetName(void) const
{
    return name_;
}

const double& PointTracker::GetAngle(void) const
{
	return angle_;
}

const PointPair& PointTracker::GetDirection(void) const
{
	return direction_;
}

void PointTracker::Visualize(void)
{
	stringstream ss;
	Mat flowMap;

	cvtColor(grayFrame_, flowMap, CV_GRAY2BGR);

	//for(int y = 0; y < flowMap.rows; y += 10)
	//	for(int x = 0; x < flowMap.cols; x += 10)
	//	{
	//		const Point2f& fxy = flow_.at<Point2f>(y, x);
	//		line(flowMap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), Scalar(0, 255, 0));
	//		circle(flowMap, Point(x, y), 2, Scalar(0, 255, 0), -1);
	//	}

	line(flowMap, Point(direction_.first), Point(direction_.second), Scalar(0, 255, 0));
	circle(flowMap, Point(direction_.first), 2, Scalar(0, 255, 0), -1);
	ss << "Processing time of MotionHistory: " << procTime_ / (cvGetTickFrequency() * 1000.0);
	VisualizerPtr->PutText(flowMap, ss.str(), Point(10, 20));
	ss.str("");

	ss << "Angle: " << angle_;
	VisualizerPtr->PutText(flowMap, ss.str(), Point(10, 40));
	ss.str("");

	VisualizerPtr->ShowImage("KeyPointMask", keyPointMask_);
	VisualizerPtr->ShowImage("FlowMap", flowMap);

	if(showBgrFlow_)
	{
		Mat flowPlanes[2], hsvPlanes[3];
		Mat magnitude, bgrFlow, hsv;
		split(flow_, flowPlanes);

		// calculate angle (hsvPlanes[0]) and magnitude (hsvPlanes[2])
		cartToPolar(flowPlanes[0], flowPlanes[1], magnitude, hsvPlanes[0], true);

		hsvPlanes[1] = Mat::ones(hsvPlanes[0].size(), CV_32F);

		// translate magnitude to range [0;1]
		double magMax;
		minMaxLoc(magnitude, NULL, &magMax);
		magnitude.convertTo(hsvPlanes[2], -1, 1.0 / magMax);

		// build hsv image
		merge(hsvPlanes, 3, hsv);

		// convert to BGR and show
		cvtColor(hsv, bgrFlow, COLOR_HSV2BGR);
		VisualizerPtr->ShowImage("Optical flow - Farneback", bgrFlow);
	}
}
