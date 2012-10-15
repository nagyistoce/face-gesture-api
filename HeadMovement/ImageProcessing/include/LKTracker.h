#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"

extern "C++" class __declspec(dllexport) LKTracker
{
public:
	LKTracker(void);
	~LKTracker(void);
};
