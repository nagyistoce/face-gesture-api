#include <fstream>
#include <iostream>
#include <opencv2//highgui/highgui.hpp>

#include "Tracer.h"
#include "StringBuilder.h"
#include "TimeStamp.h"
#include "FileSystem.h"


Tracer* Tracer::instance_ = NULL;
tbb::mutex Tracer::mutex_;


using namespace std;
using namespace cv;


Tracer::Tracer(void)
{
}


Tracer::~Tracer(void)
{
	for(int i = 0; i < int(logs_.size()); i++)
	{
		logs_[i].stream->close();
		delete logs_[i].stream;
	}
	logs_.clear();
}


Tracer* Tracer::GetInstance(void)
{
    if(instance_ == NULL)
    {
        tbb::mutex::scoped_lock lock(mutex_);
        if(instance_ == NULL)
            instance_ = new Tracer();
    }
    return instance_;
}


void Tracer::SetOutputDirectory(const string& path)
{
	TimeStamp timeStamp;
	logDirectory_ = path + "RunLogs/";
	namePrefix_ = StringBuilder() + "out." + timeStamp.AsInt() + ".";
	logHeader_ = StringBuilder() + "Execution started at " + timeStamp.GetDate() + " " + timeStamp.GetTime();
	
	if(FileSystem::DirectoryExists(logDirectory_) == false)
		FileSystem::MakeDirectory(logDirectory_);
}


void Tracer::ImShow(const string& windowName, const Mat& image)
{
    tbb::mutex::scoped_lock lock(mutex_);
    imshow(windowName, image);
    waitKey(1);
}


void Tracer::Log(const string& message, const string& file, const string& func, long line)
{
    tbb::mutex::scoped_lock lock(mutex_);
	ofstream* stream = GetLogStream("log");
	cout << message << endl << flush; 
	if(stream)
		*stream << message << endl << flush;
}


void Tracer::Warning(const string& message, const std::string& file, const string& func, long line)
{
	tbb::mutex::scoped_lock lock(mutex_);
	ofstream* stream = GetLogStream("log");
	cout << "Warning: " << message << " AT " << func << ", " << file << "/" << line << endl << flush; 
	if(stream)
		*stream << "Warning: " << message << " AT " << func << ", " << file << "/" << line << endl << flush; 
}


void Tracer::DetailedTrace(const string& message, const string& file, const string& func, long line)
{
	tbb::mutex::scoped_lock lock(mutex_);
	ofstream* stream = GetLogStream("log");
	cerr << "Detailed trace" << endl << "  Message   : " << message << endl << "  Function name: " << func << endl << "  Position: " << file << "/" << line << endl; 
	if(stream)
		*stream << "Detailed trace" << endl << "  Message   : " << message << endl << "  Function name: " << func << endl << "  Position: " << file << "/" << line << endl; 
}


ofstream* Tracer::GetLogStream(const std::string& name)
{
	for(int i = 0; i < int(logs_.size()); i++)
		if(logs_[i].name == name)
			return logs_[i].stream;

	if(logDirectory_ == "" || namePrefix_ == "")
		return NULL;


	string logPath = logDirectory_ + namePrefix_ + name + ".txt";
	LogStream logStream;
	logStream.name = name;
	logStream.stream = new ofstream(logPath.c_str(), ios::out | ios::trunc);
	logs_.push_back(logStream);

	*(logStream.stream) << logHeader_ << endl << endl;
	return logStream.stream;
}
