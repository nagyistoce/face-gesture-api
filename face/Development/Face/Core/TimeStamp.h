#pragma once

#include "FaceDef.h"


class TimeStamp
{
public:
	//! Constructor - sets current time as timestamp
	TimeStamp(void);
	TimeStamp(int time);
	~TimeStamp(void);


	int AsInt(void) const;
	int GetYear(void) const;
	int GetMonth(void) const;
	int GetDay(void) const;
	int GetHour(void) const;
	int GetMinute(void) const;
	int GetSecond(void) const;

	//! Date, formatted like 14/02/2012
	std::string GetDate(void); 

	//! Time formatted like 11:27
	std::string GetTime(void);


	//! Convert second count to a format like 3:11
	static std::string SecondCountToString(int secondCount);

private:
	void LoadTime(time_t t);

	//! Number of seconds elapsed since 1970.01.01 0:00
	int time_;
	errno_t err_;
	struct tm details_;
};
