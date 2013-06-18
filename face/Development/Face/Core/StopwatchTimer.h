#pragma once

#include <string>
#include <tbb/mutex.h>

#include "FaceDef.h"
#include "Timer.h"

#define StopwatchTimerPtr StopwatchTimer::GetInstance()


class StopwatchTimer 
{
public:
	struct TimeInfo 
	{
		float average;
		float estimated;
		float last;
		float min;
		float max;
	};

	static const int MAX_FPS = 5000;
	static StopwatchTimer* GetInstance(void);

	const TimeInfo& GetEffective(void) const;
	const TimeInfo& GetIntensity(void) const;

	int GetRunCount(void) const;
	bool HasRun(void) const;

	float GetAverageFps(void) const;
	float GetEstimatedFps(void) const;

	float GetFpsFromPeriodTime(float periodTime) const;

	void StartMeasure(void);
	void StopMeasure(void);


private:
    //! Constructor.
    StopwatchTimer(void);
    ~StopwatchTimer(void);

	void ResetTimeInfo(TimeInfo& timeInfo);
	void AddTimePeriod(TimeInfo& timeInfo, float timePeriod, int runCount);

    static StopwatchTimer* instance_;
    static tbb::mutex mutex_;

	TimeInfo effective_;
	TimeInfo intensity_;
	int runCount_;
	bool running_;
	Timer timer_;
};
