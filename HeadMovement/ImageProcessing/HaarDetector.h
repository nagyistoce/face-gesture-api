#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"

extern "C++" class __declspec(dllexport) HaarDetector
{
public:
	HaarDetector(const std::string& name);
	~HaarDetector(void);

	const std::vector<cv::Rect>& Detect(const cv::Mat& image);
	const std::vector<cv::Rect>& GetObjects(void);
	const std::string& GetName(void);

private:
	cv::CascadeClassifier cascade_;
	const std::string cascadeName_;
	const std::string name_;
	std::vector<cv::Rect> objects_;

	double scaleFactor_;
	int minNeighbors_;
	int flags_;
	cv::Size minSize_;
	cv::Size maxSize_;
};
