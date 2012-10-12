#pragma once

#include "opencv2/opencv.hpp"

extern "C++" class __declspec(dllexport) HaarDetector
{
public:
	HaarDetector(void);
	~HaarDetector(void);
};

