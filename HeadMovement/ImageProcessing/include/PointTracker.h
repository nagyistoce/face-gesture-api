#pragma once
#pragma warning(disable: 4251)

#include <utility>

#include "opencv2/opencv.hpp"

typedef std::pair<cv::Point2f, cv::Point2f> PointPair;

extern "C++" class __declspec(dllexport) PointTracker
{
public:
	PointTracker(std::string name);
	~PointTracker(void);

	void Process(const cv::Mat& frame, const cv::Mat& prevFrame, const cv::Rect& rect, const std::vector<cv::KeyPoint>& keyPoints);
	void Visualize(void);
    const std::string& GetName(void) const;
	const double& GetAngle(void) const;
	const PointPair& GetDirection(void) const;

    void InitMotionPath(cv::Point2f startPoint);

private:
    //! Virtual method for loading algorithm specific settings from the given storage.
    void LoadSettingsFromFileStorage(void);

	// Calculate angle between vector from (x1,y1) to (x2,y2) & +Y axis in degrees.
	// Essentially gives a compass reading, where N is 0 degrees and E is 90 degrees.
	double Bearing(cv::Point2f start, cv::Point2f end);

	cv::Mat grayFrame_;
	cv::Mat grayPrevFrame_;
	cv::Mat flow_;
	cv::Mat keyPointMask_;

    std::vector<cv::Point2f> motionPath_;

	PointPair direction_;
	double angle_;

    std::string name_;
	double procTime_;
    double pyrScale_;
    int levels_;
    int winsize_;
    int iterations_;
    int polyN_;
    double polySigma_;
    int flags_;

	int showBgrFlow_;
};
