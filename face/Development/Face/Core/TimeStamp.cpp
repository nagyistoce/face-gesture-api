#include <ctime>
#include <iostream>
#include <sstream>

#include "TimeStamp.h"
#include "StringBuilder.h"

using namespace std;

namespace face 
{

TimeStamp::TimeStamp(void)
{
	LoadTime(time(0));
}


TimeStamp::TimeStamp(int time) 
{
	LoadTime(time);
}


TimeStamp::~TimeStamp(void)
{
}


//! Date, formatted like 14/02/2012
std::string TimeStamp::GetDate(void)
{	
	return StringBuilder() + GetDay() + "/" + GetMonth() + "/" + GetYear();
}


//! Time formatted like 11:27
std::string TimeStamp::GetTime(void)
{
	return StringBuilder() + GetHour() + ":" + GetMinute();
}


int TimeStamp::AsInt(void) const
{
	return time_;
}


int TimeStamp::GetYear(void) const
{
	return err_ ? 0: 1900 + details_.tm_year;
}


int TimeStamp::GetMonth(void) const
{
	return err_ ? 0: details_.tm_mon+1;
}


int TimeStamp::GetDay(void) const
{	
	return err_ ? 0: details_.tm_mday;
}


int TimeStamp::GetHour(void) const
{
	return err_ ? 0: details_.tm_hour;
}


int TimeStamp::GetMinute(void) const
{
	return err_ ? 0: details_.tm_min;
}


int TimeStamp::GetSecond(void) const
{
	return err_ ? 0: details_.tm_sec;
}


string TimeStamp::SecondCountToString(int secondCount)
{
	if (secondCount < 0)
		return "0:00";
	else
	{
		int min = secondCount / 60;
		int sec = secondCount % 60;
		return StringBuilder() + min + ":" + (sec > 9 ? "" : "0") + sec;
	}
}


void TimeStamp::LoadTime(time_t t)
{
	err_ = localtime_s(&details_, &t);
	if(err_)
		time_ = 0;
	else
		time_= int(t);	
}

}
