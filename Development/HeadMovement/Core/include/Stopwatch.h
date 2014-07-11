#pragma once

//! Class for time measurement.
extern "C++" class __declspec(dllexport) Stopwatch
{
public:
	//! Constructor
	Stopwatch(void);

	//! Destructor
	~Stopwatch(void);

	//! Reset timer. 
	void Reset(void);

	//! Calculate frame per second from time delay.
	double GetFPS(void) const;

	//! Returns the elapsed time between the declaration of object (or since the last call of Reset method) and current time in second. 
	double GetElapsedTimeSec(void) const;

	//! Returns the elapsed time between the declaration of object (or since the last call of Reset method) and current time in milli second.  
	double GetElapsedTimeMilliSec(void) const;

	//! Returns the elapsed time between the declaration of object and current time in millisecond.
	double GetElapsedTimeFromStartSec(void) const;

private:
	double elapsedTimeSec_;
	double startTime_;
};

