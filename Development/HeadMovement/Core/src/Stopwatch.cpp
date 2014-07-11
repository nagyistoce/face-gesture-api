#include <opencv2/core/core.hpp>

#include "Stopwatch.h"

#ifdef OSA_WINPHONE
#include <opencv2/core/utility.hpp>
#endif

Stopwatch::Stopwatch(void)
{
	startTime_ = elapsedTimeSec_ = (double)cv::getTickCount();
}


Stopwatch::~Stopwatch(void)
{
}


void Stopwatch::Reset(void)
{
	elapsedTimeSec_ = (double)cv::getTickCount();
}


double Stopwatch::GetFPS(void) const
{
	return 1.0 / (((double)cv::getTickCount() - elapsedTimeSec_) / cv::getTickFrequency());
}


double Stopwatch::GetElapsedTimeSec(void) const
{
	return ((double)cv::getTickCount() - elapsedTimeSec_) / cv::getTickFrequency();
}


double Stopwatch::GetElapsedTimeMilliSec(void) const
{
	return ((double)cv::getTickCount() - elapsedTimeSec_) / cv::getTickFrequency() * 1000.0;
}


double Stopwatch::GetElapsedTimeFromStartSec(void) const
{
	return ((double)cv::getTickCount() - startTime_) / cv::getTickFrequency();
}
