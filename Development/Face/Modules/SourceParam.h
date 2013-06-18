#pragma once

#include <string>
#include <opencv2/core/core.hpp>

namespace face 
{

class SourceParam
{
public:
	enum Position
	{
		POSITION_FRONT = 0,
		POSITION_LEFT,
		POSITION_RIGHT,
		POSITION_UNDEFINED
	};

	SourceParam(const cv::FileStorage& configurationFs);
	SourceParam(const SourceParam& other);
	~SourceParam(void);

	std::string videoFileName;
	std::string name;
	cv::Size2f frameSize;
	int position;
	int cameraId;
	double fps;
};

}
