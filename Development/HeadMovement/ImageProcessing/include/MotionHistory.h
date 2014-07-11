#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"
#include "Stopwatch.h"

extern "C++" class __declspec(dllexport) MotionHistory
{
public:
    MotionHistory(const cv::Size& size, const int bufferSize, const double mhiDuration, const double maxTimeDelta, const double minTimeDelta, const int diffThreshold);
    ~MotionHistory(void);

    void UpdateMotionHistory(const cv::Mat& image);
	void Visualize(void);

    const cv::Mat& GetMhi(void);
    const cv::Mat& GetMotionMask(void);
	const cv::Mat& GetMotionPicture() const;

private:
	double GetWeightedAngle(cv::Mat& mag, cv::Mat& ang);

    cv::Mat *buffer_;   // ring image buffer
    cv::Mat mhi_;       // MHI
    cv::Mat motionMask_;      // valid orientation mask
    cv::Mat silh_;
	cv::Mat orientation_;
	cv::Mat motionPicture_;
	cv::Mat magnitude_;
	cv::Mat gradX;
	cv::Mat gradY;
	cv::Mat angle_;

    int lastId_;
    double procFPS_;	    //!< Processing time of the current feature extraction method.
	Stopwatch stopwatch_;
	std::vector<cv::Mat> splitChannels_;

    const int bufferSize_;
    const int diffThreshold_;
    const double mhiDuration_;
    const double maxTimeDelta_;
    const double minTimeDelta_;
};
