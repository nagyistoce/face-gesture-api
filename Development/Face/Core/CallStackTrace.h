#pragma once

#include <cstdarg>
#include <string>

namespace face 
{

#define CM_TRACE_FILE(trace_file) CallStackTrace::LogToFile(trace_file)
#define CM_TRACE_FUNC(func_name) CallStackTrace __CM_TRACE__(func_name, "()")
#define CM_TRACE_FUNC_ARG1(func_name, argfmt, arg) CallStackTrace __CM_TRACE__(func_name, argfmt, arg)
#define CM_TRACE_FUNC_ARG2(func_name, argfmt, arg1, arg2) CallStackTrace __CM_TRACE__(func_name, argfmt, arg1, arg2)

//! Abstract class for handling algorithms.
/*!
	\ingroup Core

	Usage:
    void foo()
    {
        CM_TRACE_FUNC("foo");
    }

    void bar(int x)
    {
        CM_TRACE_FUNC_ARG1("bar", "(%d)", x);
        foo();
    }

    int main()
    {
        CM_TRACE_FILE("trace.log");
        CM_TRACE_FUNC("main");
        foo();
        bar(23);
        return 0;
    }
*/
class CallStackTrace
{
public:
    //!
    explicit CallStackTrace(const std::string& func_name, const std::string& argsfmt, ...);

    //!
    ~CallStackTrace(void);

    //! special the global log file.
    void static LogToFile(const std::string& trace_file);

private:
    //!
    void LogMsg(int depth, int align, const std::string& fmt, va_list args);

    static std::string trace_file_; //!< the debug trace filename
    static std::string nest_;       //!< arbitrarily support nesting 34 deep for no particular reason
    static int depth_;              //!< function call stack depth
};

}
