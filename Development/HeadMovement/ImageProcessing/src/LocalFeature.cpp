#include "Definitions.h"
#include "LocalFeature.h"
#include "Visualizer.h"
#include <iomanip>

using namespace std;
using namespace cv;

LocalFeature::LocalFeature(const string& name, const string& type)
:	name_(name),
    type_(type),
    procFps_(0.0),
    centerOfPts_(Point2f(0.0f, 0.0f))
{
}

LocalFeature::~LocalFeature(void)
{
}

void LocalFeature::SetFrame(const Mat& frame)
{
    frame_ = frame.clone();
    if(frame_.channels() != 3)
        cvtColor(frame_, frame_, CV_GRAY2BGR);

    keyPoints.clear();
}

void* LocalFeature::Run(void)
{
	stopwatch_.Reset();
    //procTime_ = (double)cvGetTickCount();
    centerOfPts_ = Point2f(0.0f, 0.0f);

    if(!frame_.empty())
    {
        Process();
        DrawFeatures();

        if(!keyPoints.empty())
        {
            for (size_t i = 0; i < keyPoints.size(); i++)
            {
                centerOfPts_.x += keyPoints[i].pt.x;
                centerOfPts_.y += keyPoints[i].pt.y;
            }

            centerOfPts_.x /= keyPoints.size();
            centerOfPts_.y /= keyPoints.size();
        }
    }

	procFps_ = stopwatch_.GetFPS();
    //procTime_ = (double)cvGetTickCount() - procTime_;

    return reinterpret_cast<void*>(0);
}


void LocalFeature::Visualize(void)
{
    stringstream ss;

    ss << name_ << ": " << cvRound(procFps_) << " FPS.";
    VisualizerPtr->PutText(frame_, ss.str(), Point(10, 20));
    ss.str("");

    ss << "Number of detected keypoints: " << keyPoints.size();
    VisualizerPtr->PutText(frame_, ss.str(), Point(10, 40));
    ss.str("");

    //circle(frame_, Point(centerOfPts_), 5, Scalar(0, 0, 255), -1);
    //ss << "Center of points: (" << centerOfPts_.x << ", " << centerOfPts_.y << ")";
    //VisualizerPtr->PutText(frame_, ss.str(), Point(10, 60));
    //ss.str("");

    VisualizerPtr->ShowImage("Local Feature Extractor - " + name_, frame_);
}


const string& LocalFeature::GetName(void)
{
    return name_;
}

const string& LocalFeature::GetType(void)
{
    return type_;
}

double LocalFeature::GetProcTime(void)
{
    return procFps_;
}
