//////////////////////////////////////////////////////////////////////////////
// Timer.cpp
// =========
// High Resolution Timer.
// This timer is able to measure the elapsed time with 1 micro-second accuracy
// in both Windows, Linux and Unix system
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2003-01-13
// UPDATED: 2006-01-13
//
// Copyright (c) 2003 Song Ho Ahn
//////////////////////////////////////////////////////////////////////////////

#define WIN32 1

#include <cstdlib>
#include <cmath>
#include "Timer.h"


///////////////////////////////////////////////////////////////////////////////
// constructor
///////////////////////////////////////////////////////////////////////////////
Timer::Timer(void)
:   stopped_(0),
    startTimeInMicroSec_(0),
    endTimeInMicroSec_(0)
{
#ifdef WIN32
    QueryPerformanceFrequency(&frequency_);
    startCount_.QuadPart = 0;
    endCount_.QuadPart = 0;
#else
    startCount_.tv_sec = startCount_.tv_usec = 0;
    endCount_.tv_sec = endCount_.tv_usec = 0;
#endif
    Start();
}


///////////////////////////////////////////////////////////////////////////////
// destructor
///////////////////////////////////////////////////////////////////////////////
Timer::~Timer(void)
{
	// nothing to do here
}


///////////////////////////////////////////////////////////////////////////////
// start timer.
// startCount_ will be set at this point.
///////////////////////////////////////////////////////////////////////////////
void Timer::Start(void)
{
    stopped_ = 0; // reset stop flag
#ifdef WIN32
    QueryPerformanceCounter(&startCount_);
#else
    gettimeofday(&startCount_, NULL);
#endif
}


///////////////////////////////////////////////////////////////////////////////
// stop the timer.
// endCount_ will be set at this point.
///////////////////////////////////////////////////////////////////////////////
void Timer::Stop(void)
{
    stopped_ = 1; // set timer stopped flag
#ifdef WIN32
    QueryPerformanceCounter(&endCount_);
#else
    gettimeofday(&endCount_, NULL);
#endif
}


///////////////////////////////////////////////////////////////////////////////
// compute elapsed time in micro-second resolution.
// other GetElapsedTime will call this first, then convert to correspond resolution.
///////////////////////////////////////////////////////////////////////////////
double Timer::GetElapsedTimeInMicroSec(void)
{
#ifdef WIN32
    if(!stopped_)
    {
		QueryPerformanceCounter(&endCount_);
    }

    startTimeInMicroSec_ = startCount_.QuadPart * (1000000.0 / frequency_.QuadPart);
    endTimeInMicroSec_ = endCount_.QuadPart * (1000000.0 / frequency_.QuadPart);
#else    
    if(!stopped_) 
    {        
        gettimeofday(&endCount_, NULL);
    }

    startTimeInMicroSec_ = (startCount_.tv_sec * 1000000.0) + startCount_.tv_usec;
    endTimeInMicroSec_ = (endCount_.tv_sec * 1000000.0) + endCount_.tv_usec;
#endif

    return endTimeInMicroSec_ - startTimeInMicroSec_;
}


///////////////////////////////////////////////////////////////////////////////
// divide elapsedTimeInMicroSec by 1000
///////////////////////////////////////////////////////////////////////////////
double Timer::GetElapsedTimeInMilliSec(void)
{
    return GetElapsedTimeInMicroSec() * 0.001;
}


///////////////////////////////////////////////////////////////////////////////
// divide elapsedTimeInMicroSec by 1000000
///////////////////////////////////////////////////////////////////////////////
double Timer::GetElapsedTimeInSec(void)
{
    return GetElapsedTimeInMicroSec() * 0.000001;
}


///////////////////////////////////////////////////////////////////////////////
// same as GetElapsedTimeInSec()
///////////////////////////////////////////////////////////////////////////////
double Timer::GetElapsedTime(void)
{
    return GetElapsedTimeInMicroSec() * 0.000001;
}


timeval Timer::GetElapsedTimeInTimeval(void)
{
	timeval tvTime;
	double tmp = GetElapsedTimeInMilliSec();

	tvTime.tv_sec = (long)(tmp * 0.001);
	tvTime.tv_usec = (long)(tmp - (tvTime.tv_sec * 1000));

	return tvTime;
}


timeval operator+(timeval T1, timeval T2)
{
	timeval tvREsult;
	tvREsult.tv_sec = T1.tv_sec + T2.tv_sec;
	tvREsult.tv_usec = T1.tv_usec + T2.tv_usec;

	if(tvREsult.tv_usec >= 1000)
	{
		++tvREsult.tv_sec;
		tvREsult.tv_usec -= 1000;
	}

	return tvREsult;
}


timeval operator-(timeval T1, timeval T2)
{
	timeval tvREsult;
	tvREsult.tv_sec	= T1.tv_sec - T2.tv_sec;
	tvREsult.tv_usec= T1.tv_usec - T2.tv_usec;

	if(tvREsult.tv_usec < 0)
	{
		--tvREsult.tv_sec;
		tvREsult.tv_usec += 1000;
	}

	return tvREsult;
}


timeval operator+(timeval T1, long usec)
{
	timeval tvTempTime;
	tvTempTime.tv_sec = long(floor(double(usec) / 1000.0));
	tvTempTime.tv_usec = usec - 1000 * tvTempTime.tv_sec;

	return T1 + tvTempTime;
}


timeval operator-(timeval T1, long usec)
{
	timeval tvTempTime;
	tvTempTime.tv_sec = long(floor(double(usec) / 1000.0));
	tvTempTime.tv_usec = usec - 1000 * tvTempTime.tv_sec;

	return T1 - tvTempTime;
}
