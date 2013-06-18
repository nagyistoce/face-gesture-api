#include "SourceParam.h"

namespace face 
{

SourceParam::SourceParam(const cv::FileStorage& configurationFs)
:	videoFileName(""),
	name("front"),
	position(SourceParam::POSITION_UNDEFINED),
	cameraId(0),
	frameSize(cv::Size2f(640.0f, 480.0f)),
	fps(30.0)
{
	if(!configurationFs["name"].empty() && configurationFs["name"].isString())
	{
		configurationFs["name"] >> name;
	}

	if(!configurationFs["position"].empty() && configurationFs["position"].isString())
	{
		std::string positionStr;
		configurationFs["position"] >> positionStr;

		if(positionStr == "front")
			position = SourceParam::POSITION_FRONT;
		else if(positionStr == "left")
			position = SourceParam::POSITION_LEFT;
		else if(positionStr == "right")
			position = SourceParam::POSITION_RIGHT;
		else
			position = SourceParam::POSITION_UNDEFINED;
	}

	if(!configurationFs["cameraId"].empty() && configurationFs["cameraId"].isInt())
	{
		configurationFs["cameraId"] >> cameraId;
	}

	if(!configurationFs["videoFileName"].empty() && configurationFs["videoFileName"].isString())
	{
		configurationFs["videoFileName"] >> videoFileName;
	}

	if(!configurationFs["width"].empty() && configurationFs["width"].isReal())
	{
		configurationFs["width"] >> frameSize.width;
	}

	if(!configurationFs["height"].empty() && configurationFs["height"].isReal())
	{
		configurationFs["height"] >> frameSize.height;
	}

	if(!configurationFs["fps"].empty() && configurationFs["fps"].isReal())
	{
		configurationFs["fps"] >> fps;
	}
}


SourceParam::SourceParam(const SourceParam& other)
:	videoFileName(other.videoFileName),
	name(other.name),
	position(other.position),
	cameraId(other.cameraId),
	frameSize(other.frameSize),
	fps(other.fps)
{
}


SourceParam::~SourceParam(void)
{
}

}
