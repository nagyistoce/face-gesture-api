#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"

extern "C++" class __declspec(dllexport) HeadMovementAlgorithm
{
public:
	HeadMovementAlgorithm(void);
	~HeadMovementAlgorithm(void);

	void ProcessAlgorithm(void);

private:
	cv::VideoCapture videoCapture_;
	cv::Mat frame_;
};
