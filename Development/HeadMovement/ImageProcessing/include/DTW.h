#pragma once
#pragma warning( disable: 4251 4244 ) // Disable needs to have dll-interface, and int to float...

#include "opencv2/opencv.hpp"

//! Class for measuring similarity between two sequences which may vary in time or speed.
/*!
	Dynamic time warping (DTW) is an algorithm for measuring similarity between two sequences which may vary in time or speed.

	In general, DTW is a method that allows a computer to find an optimal match between two given sequences (e.g. time series) with certain restrictions. The sequences are "warped" non-linearly in the time dimension to determine a measure of their similarity independent of certain non-linear variations in the time dimension. This sequence alignment method is often used in the context of hidden Markov models.


	\ingroup Core
*/
extern "C++" class __declspec(dllexport) DTW
{
public:
	//! Constructor
	DTW(void);

	//! Destructor
	~DTW(void);

    double DoDtw(const std::vector<double>& seq1, const std::vector<double>& seq2, double& err);

private:
    double Distance(double p1, double p2, double pd);

    cv::Mat dtwMap_;
    double procTime_;
};
