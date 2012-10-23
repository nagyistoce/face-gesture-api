#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"

extern "C++" class __declspec(dllexport) PointTracker
{
public:
	PointTracker(std::string name);
	~PointTracker(void);

	void Process(const cv::Mat& frame, const cv::Mat& prevFrame, const cv::Rect& rect, const std::vector<cv::KeyPoint>& keyPoints);
	void Visualize(void);
    std::string GetName(void) const;

private:
    //! Virtual method for loading algorithm specific settings from the given storage.
    void LoadSettingsFromFileStorage(void);

	cv::Mat grayFrame_;
	cv::Mat grayPrevFrame_;
	cv::Mat flow_;

    std::string name_;
	double procTime_;
    double pyrScale_;
    int levels_;
    int winsize_;
    int iterations_;
    int polyN_;
    double polySigma_;
    int flags_;
};
