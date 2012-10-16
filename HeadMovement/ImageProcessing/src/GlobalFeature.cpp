#include "Definitions.h"
#include "GlobalFeature.h"
#include "Visualizer.h"

using namespace std;
using namespace cv;

GlobalFeature::GlobalFeature(const string& name, const string& type)
:	name_(name),
    type_(type),
	procTime_(0.0),
    avgAngle_(0.0)
{
}

GlobalFeature::~GlobalFeature(void)
{
}

void GlobalFeature::SetFrame(const Mat& frame)
{
	if(frame.channels() != 3)
		cvtColor(frame, frame_, CV_GRAY2BGR);
	else
		frame_ = frame.clone();
}

void* GlobalFeature::Run(void)
{
	procTime_ = (double)cvGetTickCount();
	if(!frame_.empty())
	{
		Process();
		DrawFeatures();
        //Visualize();
	}
	procTime_ = (double)cvGetTickCount() - procTime_;

	return reinterpret_cast<void*>(0);
}


void GlobalFeature::Visualize(void)
{
    char buffer[500];

    sprintf_s(buffer, 500, "Processing time of %s: %.2lf ms.", name_.c_str(), procTime_ / (cvGetTickFrequency() * 1000.0));
    VisualizerPtr->PutText(frame_, buffer, Point(10, 20));

    sprintf_s(buffer, 500, "Number of detected keypoints: %d.", keyPoints.size());
    VisualizerPtr->PutText(frame_, buffer, Point(10, 40));

    avgAngle_ = 0.0;
    for (size_t i = 0; i < keyPoints.size(); i++)
        avgAngle_ += keyPoints[i].angle;
    
    avgAngle_ /= keyPoints.size();

    stringstream ss;
    ss << "AVG Angle: " << avgAngle_;
    VisualizerPtr->PutText(frame_, ss.str(), Point(10, 60));

    VisualizerPtr->ShowImage(name_, frame_);
}


const string& GlobalFeature::GetName(void)
{
    return name_;
}

const string& GlobalFeature::GetType(void)
{
    return type_;
}

double GlobalFeature::GetProcTime(void)
{
    return procTime_;
}
