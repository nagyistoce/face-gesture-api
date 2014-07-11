#include "Definitions.h"
#include "GlobalFeature.h"
#include "Visualizer.h"
#include <iomanip>

using namespace std;
using namespace cv;

GlobalFeature::GlobalFeature(const string& name, const string& type)
:	name_(name),
    type_(type),
	procFps_(0.0)
{
}

GlobalFeature::~GlobalFeature(void)
{
}

void GlobalFeature::SetFrame(const Mat& frame)
{
    frame_ = frame.clone();

	if(frame_.channels() != 3)
		cvtColor(frame_, frame_, CV_GRAY2BGR);

    features.clear();
}

void* GlobalFeature::Run(void)
{
	stopwatch_.Reset();
	//procTime_ = (double)cvGetTickCount();
	if(!frame_.empty())
	{
        ProcessInit();
		Process();
		DrawFeatures();
	}
	//procTime_ = (double)cvGetTickCount() - procTime_;
	procFps_ = stopwatch_.GetFPS();

	return reinterpret_cast<void*>(0);
}


void GlobalFeature::Visualize(void)
{
    stringstream ss;

    ss << name_ << ": " << cvRound(procFps_) << " FPS.";
    VisualizerPtr->PutText(frame_, ss.str(), Point(10, 20));
    ss.str("");

    ss << "Number of features: " << features.size();
    VisualizerPtr->PutText(frame_, ss.str(), Point(10, 40));
    ss.str("");

    VisualizerPtr->ShowImage("Global Feature Extractor - " + name_, outputFrame_);
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
    return procFps_;
}
