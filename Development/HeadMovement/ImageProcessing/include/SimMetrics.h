#pragma once
#pragma warning( disable: 4251 ) // Disable needs to have dll-interface...

#include "opencv2/core/core.hpp"

//! Class for measuring similarity between variuos types of object for image processing

extern "C++" class __declspec(dllexport) SimMetrics
{
public:

	enum Method
	{
		DEV_LINEAR = 0,
		DEV_SQD
	};

	SimMetrics();
	~SimMetrics();

	double DtwSeq(const std::vector<double>& seq1, const std::vector<double>& seq2, double& err);

	double FvdTransform(const std::vector<cv::Point>& contour1, const std::vector<cv::Point>& contour2);

	double GetSdgoValue(const std::vector<cv::Point>& contour1, const std::vector<cv::Point>& contour2, int id);

	double GetRdrValue(const std::vector<cv::Point>& contour1, const std::vector<cv::Point>& contour2);

	double GetRafValue(const std::vector<cv::Point>& contour1, const std::vector<cv::Point>& contour2);

	double CompareSeq(const std::vector<float>& seq1, std::vector<float>& seq2, float itRange, int methodType);
	
	cv::Mat dtwMap;

	cv::Mat imgGradT; 

	cv::Mat imgGradR;

	cv::Mat imgDistRate;

private:

	double GetDistanceGr(double p1, double p2, double pd);

	int graphW_;
	int graphH_;


};
