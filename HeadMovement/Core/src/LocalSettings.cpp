#include <fstream>
#include "opencv2/opencv.hpp"

#include "Definitions.h"
#include "LocalSettings.h"

using namespace std;
using namespace cv;

LocalSettings::LocalSettings(void)
{
	string localSettings("../LocalSettings.txt");
	ifstream fileStream(localSettings.c_str());

	if (!fileStream.is_open()) 
		CV_Error(1, "Local settings file '" + localSettings + "' does not exist!");

	getline(fileStream, dataDirectory_);

	inputDirectory_ = dataDirectory_ + "input/";
	processXmlFileName_ = dataDirectory_ + "xml/Process.default.xml";
    motionDirectory_  = dataDirectory_ + "xml/Motion/";
	gestureDirectory_ = dataDirectory_ + "xml/Gesture/";
    featureExtractorDirectory_  = dataDirectory_ + "xml/FeatureExtractor/";
	detectorDirectory_ = dataDirectory_ + "xml/Detector/";

	fileStream.close();
}

LocalSettings::~LocalSettings(void)
{
}

LocalSettings* LocalSettings::GetInstance(void)
{
	static LocalSettings* localSettings = new LocalSettings();
	return localSettings;
}

string LocalSettings::GetDataDirectory(void) const
{
	return dataDirectory_;
}

string LocalSettings::GetInputDirectory(void) const
{
	return inputDirectory_;
}

string LocalSettings::GetFeatureExtractorDirectory(void) const
{
	return featureExtractorDirectory_;
}

string LocalSettings::GetDetectorDirectory(void) const
{
	return detectorDirectory_;
}

string LocalSettings::GetProcessXmlFileName(void) const
{
    return processXmlFileName_;
}

string LocalSettings::GetMotionDirectory(void) const
{
    return motionDirectory_;
}

string LocalSettings::GetGestureDirectory(void) const
{
	return gestureDirectory_;
}
