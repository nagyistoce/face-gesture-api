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
    //grayFrame_ = grayFrame_(rect);
    //grayPrevFrame_ = grayPrevFrame_(rect);

	calcOpticalFlowFarneback(grayPrevFrame_, grayFrame_, flow_, pyrScale_, levels_, winsize_, iterations_, polyN_, polySigma_, flags_);

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
