#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"

class PointTracker;

extern "C++" class __declspec(dllexport) MotionHistory
{
public:
    MotionHistory(const cv::Size& size);
    ~MotionHistory(void);

    void UpdateMotionHistory(const cv::Mat& image, int diffThreshold);
    void PredictMotionVectors(const cv::Mat& frame, const cv::Mat& prevFrame, const cv::Rect& rect, const std::vector<cv::Point2f>& points);
	void Visualize(void);

    const cv::Mat& GetMhi(void);
    const cv::Mat& GetMask(void);

private:
    PointTracker* pointTracker_;
    cv::Mat *buffer_;   // ring image buffer
    cv::Mat mhi_;       // MHI
    cv::Mat orient_;    // orientation
    cv::Mat mask_;      // valid orientation mask
    cv::Mat segmask_;   // motion segmentation map

    int lastId_;
};
