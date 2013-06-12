#include "HaarDetectorParam.h"

namespace face 
{

HaarDetectorParam::HaarDetectorParam(const cv::FileStorage& configurationFs)
:	cascadeName(""),
	scaleFactor(1.2),
	imgScaleFactor(1.0),
	invImgScaleFactor(1.0),
	minNeighbors(2),
	flags(0),
	minSize(cv::Size()),
	maxSize(cv::Size()),
	winSize(cv::Size(21, 21)),
	maxLevel(3),
	criteria(cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.01)),
	LKflags(0),
	minEigThreshold(1e-4)
{
	//TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);
	//Size winSize(5, 5);
	//calcOpticalFlowPyrLK(prevFrame, frameIn, prevPoints, nextPoints, status, err, winSize, 3, termcrit, 0, 0.001);

	if(!configurationFs["cascadeName"].empty() && configurationFs["cascadeName"].isString())
	{
		configurationFs["cascadeName"] >> cascadeName;
	}

	if(!configurationFs["scaleFactor"].empty() && configurationFs["scaleFactor"].isReal())
	{
		configurationFs["scaleFactor"] >> scaleFactor;
	}

	if(!configurationFs["imgScaleFactor"].empty() && configurationFs["imgScaleFactor"].isReal())
	{
		configurationFs["imgScaleFactor"] >> imgScaleFactor;

		if(imgScaleFactor >= 1.0)
			imgScaleFactor = 1.0;
		else if(imgScaleFactor <= 0.0)
			imgScaleFactor = 0.1;

		invImgScaleFactor = 1.0 / imgScaleFactor;
	}

	if(!configurationFs["minNeighbors"].empty() && configurationFs["minNeighbors"].isInt())
	{
		configurationFs["minNeighbors"] >> minNeighbors;
	}

	if(!configurationFs["flags"].empty() && configurationFs["flags"].isInt())
	{
		configurationFs["flags"] >> flags;
	}

	if(!configurationFs["minWidth"].empty() && !configurationFs["minHeight"].empty() && 
		configurationFs["minWidth"].isInt() && configurationFs["minHeight"].isInt())
	{
		int minWidth, minHeight;
		configurationFs["minWidth"] >> minWidth;
		configurationFs["minHeight"] >> minHeight;
		minSize = cv::Size(minWidth, minHeight);
	}

	if(!configurationFs["maxWidth"].empty() && !configurationFs["maxHeight"].empty() && 
		configurationFs["maxWidth"].isInt() && configurationFs["maxHeight"].isInt())
	{
		int maxWidth, maxHeight;
		configurationFs["maxWidth"] >> maxWidth;
		configurationFs["maxHeight"] >> maxHeight;
		maxSize = cv::Size(maxWidth, maxHeight);
	}
}


HaarDetectorParam::HaarDetectorParam(const HaarDetectorParam& other)
:	cascadeName(other.cascadeName),
	scaleFactor(other.scaleFactor),
	imgScaleFactor(other.imgScaleFactor),
	invImgScaleFactor(other.invImgScaleFactor),
	minNeighbors(other.minNeighbors),
	flags(other.flags),
	minSize(other.minSize),
	maxSize(other.maxSize),
	winSize(other.winSize),
	maxLevel(other.maxLevel),
	criteria(other.criteria),
	LKflags(other.LKflags),
	minEigThreshold(other.minEigThreshold)
{

}


HaarDetectorParam::~HaarDetectorParam(void)
{
}

}
