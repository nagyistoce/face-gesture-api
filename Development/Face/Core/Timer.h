//////////////////////////////////////////////////////////////////////////////
// Timer.h
// =======
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
#pragma once

#include <windows.h>
#include <WinSock.h>

#include "FaceDef.h"

namespace face 
{

timeval operator+(timeval T1, timeval T2);
timeval operator-(timeval T1, timeval T2);
timeval operator+(timeval T1, long usec);
timeval operator-(timeval T1, long usec);

//! Class for measuring time.
class Timer
{
public:
    Timer(void);                            //!< Default constructor.
    ~Timer(void);                           //!< Default destructor.

    void Start(void);                       //!< Start timer.
    void Stop(void);                        //!< Stop the timer.

    double GetElapsedTime(void);            //!< Get elapsed time in second.
    double GetElapsedTimeInSec(void);       //!< Get elapsed time in second (same as GetElapsedTime).
    double GetElapsedTimeInMilliSec(void);  //!< Get elapsed time in milli-second.
    double GetElapsedTimeInMicroSec(void);  //!< Get elapsed time in micro-second.
	timeval GetElapsedTimeInTimeval(void);	//!< Get elapsed time in timeval struct.


private:
    double startTimeInMicroSec_;             //!< Starting time in micro-second.
    double endTimeInMicroSec_;               //!< Ending time in micro-second.
    int stopped_;                            //!< Stop flag.

#ifdef WIN32
    LARGE_INTEGER frequency_;                //!< Ticks per second.
    LARGE_INTEGER startCount_;               //!<
    LARGE_INTEGER endCount_;                 //!<
#else
    timeval startCount_;                     //!<
    timeval endCount_;                       //!<
#endif

};

}
