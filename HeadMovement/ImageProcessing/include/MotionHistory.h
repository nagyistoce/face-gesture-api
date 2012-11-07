#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"

extern "C++" class __declspec(dllexport) MotionHistory
{
public:
    MotionHistory(const cv::Size& size, const int bufferSize, const double mhiDuration, const double maxTimeDelta, const double minTimeDelta, const int diffThreshold);
    ~MotionHistory(void);

    void UpdateMotionHistory(const cv::Mat& image);
	void Visualize(void);

    const cv::Mat& GetMhi(void);
    const cv::Mat& GetMask(void);

private:
    cv::Mat *buffer_;   // ring image buffer
    cv::Mat mhi_;       // MHI
    cv::Mat orient_;    // orientation
    cv::Mat mask_;      // valid orientation mask
    cv::Mat segmask_;   // motion segmentation map

    int lastId_;

    const int bufferSize_;
    const int diffThreshold_;
    const double mhiDuration_;
    const double maxTimeDelta_;
    const double minTimeDelta_;
};
