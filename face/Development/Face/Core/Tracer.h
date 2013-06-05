#pragma once

#include <vector>
#include <string>
#include <opencv2/core/core.hpp>
#include <tbb/mutex.h>

#include "FaceDef.h"
#include "ExceptionDescriptor.h"
#include "CallStackTrace.h"

#define LOG(message) static_cast<void>(Tracer::GetInstance()->Log((StringBuilder() + message), __FILE__, __FUNCTION__, __LINE__))
#define ASSERT(object) static_cast<void>(!(object) ? (Tracer::GetInstance()->Log(("Assertion failed: " #object), __FILE__, __FUNCTION__, __LINE__), throw (ExceptionError("Assertion failed: '" #object "'!"))) : 0)
#define IMSHOW(windowName, image) static_cast<void>(Tracer::GetInstance()->ImShow(windowName, image))

#if defined _DEBUG
#  define TRACE(message) static_cast<void>(Tracer::GetInstance()->Log((StringBuilder() + message), __FILE__, __FUNCTION__, __LINE__))
#  define WARNING(message) static_cast<void>(Tracer::GetInstance()->Warning((StringBuilder() + message), __FILE__, __FUNCTION__, __LINE__))
#  define DETAILED_TRACE(message) static_cast<void>(Tracer::GetInstance()->DetailedTrace((StringBuilder() + message), __FILE__, __FUNCTION__, __LINE__))
#else
#  define TRACE(message) static_cast<void>(0)
#  define WARNING(message) static_cast<void>(0)
#  define DETAILED_TRACE(message) static_cast<void>(0)
#endif


//! Logger singleton
/*!
	Provides runtime and development-time logging functionality. All logs should be created using this object.
*/
class Tracer
{
public:
	static Tracer* GetInstance(void);
	
    void ImShow(const std::string& windowName, const cv::Mat& image);

    /*! Log to console

	The log is duplicated to a logfile if the log directory is specified.
	
	Should be called using a macro to provide the meta parameters automatically.
	*/
	void Log(const std::string& message, const std::string& file, const std::string& func, long line);


	/*! Log warning message to console

	The log is duplicated to a logfile if the log directory is specified.
	
	Should be called using a macro to provide the meta parameters automatically.
	*/
	void Warning(const std::string& message, const std::string& file, const std::string& func, long line);
	

	/*! Log detailed trace to the console 

	Prints the given message to the console with the in-code position information (line number, etc..).
	The log is duplicated to a logfile if the log directory is specified.

	Should be called using a macro to provide the meta parameters automatically.
	*/
	void DetailedTrace(const std::string& message, const std::string& file, const std::string& func, long line);

	void SetOutputDirectory(const std::string& path);

private:
    static Tracer* instance_;
    static tbb::mutex mutex_;

	struct LogStream
	{
		std::string name;
		std::ofstream* stream;
	};

	Tracer(void);
	~Tracer(void);

	std::ofstream* GetLogStream(const std::string& name);

	std::string logDirectory_;
	std::string namePrefix_;
	std::string logHeader_;
	std::vector<LogStream> logs_;
};
