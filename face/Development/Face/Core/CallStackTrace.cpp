#include "CallStackTrace.h"
#include "TimeStamp.h"
#include "GlobalSettings.h"
#include "ExceptionDescriptor.h"


using namespace std;


// arbitrarily support nesting 34 deep for no particular reason
string CallStackTrace::nest_ = "| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | ";
string CallStackTrace::trace_file_ = "";
int CallStackTrace::depth_ = 0;


CallStackTrace::CallStackTrace(const string& func_name, const string& argsfmt, ...)
{
    char fmt[256] = { 0 };
    sprintf_s(fmt, 200, "%s%s", func_name, argsfmt);

    va_list arglist;
    va_start(arglist, argsfmt);
    LogMsg(depth_, depth_ * 2, fmt, arglist);
    va_end(arglist);

    ++depth_;
}


CallStackTrace::~CallStackTrace(void)
{
    --depth_;
}


void CallStackTrace::LogToFile(const string& trace_file)
{
    trace_file_ = GlobalSettingsConstPtr->GetDirectories().output + trace_file;
}


void CallStackTrace::LogMsg(int depth, int align, const string& fmt, va_list args)
{
    FILE* fp;
    if(fopen_s(&fp, trace_file_.c_str(), "a+") == 0)
        ExceptionError("Could not open CallStackTrace file, filename=\"" + trace_file_ + "\".");

    TimeStamp timeStamp;
    string ts = timeStamp.GetYear() + timeStamp.GetMonth() + timeStamp.GetDay() + "." + timeStamp.GetHour() + timeStamp.GetMinute() + timeStamp.GetSecond();
    unsigned int len = fprintf(fp, "%s %*.*s> (%d)", ts, 2 * depth, 2 * depth, nest_, depth);

    len += vfprintf(fp, fmt.c_str(), args);
    len += fwrite("\n", 1, 1, fp);
    fflush(fp);
    fclose(fp);
}
