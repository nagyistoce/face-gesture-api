#pragma once

#include <string>
#include <opencv2/core/core.hpp>

namespace face 
{

class HaarDetectorParam
{
public:
	HaarDetectorParam(const cv::FileStorage& configurationFs);
	HaarDetectorParam(const HaarDetectorParam& other);
	~HaarDetectorParam(void);

	std::string cascadeName;
	double scaleFactor;
	double imgScaleFactor;
	double invImgScaleFactor;
	int minNeighbors;
	int flags;
	cv::Size minSize;
	cv::Size maxSize;

	cv::Size winSize;
	int maxLevel;
	cv::TermCriteria criteria;
	int LKflags;
	double minEigThreshold;
};

}
