#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"

extern "C++" class __declspec(dllexport) PointTracker
{
public:
	PointTracker(std::string name);
	~PointTracker(void);

    std::string GetName(void) const;

private:
    //! Virtual method for loading algorithm specific settings from the given storage.
    void LoadSettingsFromFileStorage(void);

    std::string name_;

    std::vector<cv::Point2f> nextPoints_;
    std::vector<uchar> status_;
    std::vector<float> error_;

    cv::Size winSize_;
    int maxLevel_;
    int criteriaType_;
    int criteriaMaxCount_;
    double criteriaEpsilon_;
    cv::TermCriteria criteria_;
    int flags_;
    double minEigThreshold_;
};
