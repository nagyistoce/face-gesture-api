#include "StopwatchTimer.h"
#include "Tracer.h"

using namespace std;

namespace face 
{

StopwatchTimer* StopwatchTimer::instance_ = NULL;
tbb::mutex StopwatchTimer::mutex_;

StopwatchTimer* StopwatchTimer::GetInstance(void)
{
    if(instance_ == NULL)
    {
        tbb::mutex::scoped_lock lock(mutex_);
        if(instance_ == NULL)
            instance_ = new StopwatchTimer();
    }
    return instance_;
}


StopwatchTimer::StopwatchTimer(void)
:	runCount_(0),
    running_(0)
{
    ResetTimeInfo(intensity_);
    ResetTimeInfo(effective_);
}


StopwatchTimer::~StopwatchTimer(void)
{
}


const StopwatchTimer::TimeInfo& StopwatchTimer::GetEffective(void) const 
{
	return effective_;
}


const StopwatchTimer::TimeInfo& StopwatchTimer::GetIntensity(void) const 
{
	return intensity_;
}


int StopwatchTimer::GetRunCount(void) const
{
	return runCount_;
}


bool StopwatchTimer::HasRun(void) const
{
	return runCount_ > 0;
}


float StopwatchTimer::GetAverageFps(void) const
{
	return GetFpsFromPeriodTime(effective_.average);
}


float StopwatchTimer::GetEstimatedFps(void) const
{
	return GetFpsFromPeriodTime(effective_.estimated);
}


void StopwatchTimer::StartMeasure(void)
{
	AddTimePeriod(intensity_, float(timer_.GetElapsedTimeInSec()), runCount_);

	timer_.Stop();
	timer_.Start();

	running_ = true;
}


void StopwatchTimer::StopMeasure(void)
{
	if(running_ == false)
		return;

	AddTimePeriod(effective_, float(timer_.GetElapsedTimeInSec()), runCount_);
	runCount_++;

	running_ = false;
}


float StopwatchTimer::GetFpsFromPeriodTime(float periodTime) const 
{
	if (periodTime != 0 && periodTime < (1.0f / MAX_FPS)) 
		return float(MAX_FPS);
	return 1.0f / periodTime;
}


void StopwatchTimer::ResetTimeInfo(StopwatchTimer::TimeInfo& timeInfo) 
{
	timeInfo.estimated = 0;
	timeInfo.last = 0;
	timeInfo.max = 0;
	timeInfo.min = 0;
}


void StopwatchTimer::AddTimePeriod(StopwatchTimer::TimeInfo& timeInfo, float timePeriod, int runCount)
{
	timeInfo.last = timePeriod;

	if (runCount > 0) {
		timeInfo.average = (timeInfo.average * runCount + timePeriod) / (runCount + 1);
		timeInfo.estimated = timePeriod*0.3f + timeInfo.estimated * 0.6f + timeInfo.average * 0.1f;
	}
	else {
		timeInfo.average = timePeriod;
		timeInfo.estimated = timePeriod;
	}

	if (timeInfo.min > timePeriod || runCount == 0)
		timeInfo.min = timePeriod;
	if (timeInfo.max < timePeriod)
		timeInfo.max = timePeriod;
}

}
