#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"

class HaarDetector;

class HeadMovementAlgorithm
{
public:
	HeadMovementAlgorithm(void);
	~HeadMovementAlgorithm(void);

	void ProcessAlgorithm(void);

private:
	void DrawObjects(cv::Mat& image, const std::vector<cv::Rect>& objects);

	std::vector<cv::Rect> faces_;

	HaarDetector* faceDetector_;
	cv::VideoCapture videoCapture_;
	cv::Mat frame_;
	cv::Mat prevFrame_;
};
