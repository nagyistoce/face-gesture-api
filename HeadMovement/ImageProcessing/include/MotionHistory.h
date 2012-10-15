#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"

extern "C++" class __declspec(dllexport) MotionHistory
{
public:
    MotionHistory(const cv::Size& size);
    ~MotionHistory(void);

    void UpdateMotionHistory(const cv::Mat& image, int diffThreshold);

    const cv::Mat& GetMhi(void);
    const cv::Mat& GetMask(void);

private:
    cv::Mat *buffer_;   // ring image buffer
    cv::Mat mhi_;       // MHI
    cv::Mat orient_;    // orientation
    cv::Mat mask_;      // valid orientation mask
    cv::Mat segmask_;   // motion segmentation map

    int lastId_;
};
