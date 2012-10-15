#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"

class HaarDetector;
class MotionHistory;
class GlobalFeature;
class LocalFeature;
class ImageFrame;

typedef std::map<std::string, GlobalFeature*> GlobalFeaturePool;
typedef std::map<std::string, LocalFeature*> LocalFeaturePool;
typedef std::map<std::string, HaarDetector*> HaarDetectorPool;

class HeadMovementAlgorithm
{
public:
	HeadMovementAlgorithm(void);
	~HeadMovementAlgorithm(void);

	void Process(void);

private:
    //! Method for loading algorithm specific settings from the given storage.
	/*!
		\param fileStorage An already open storage from which anything specific can be read.
	*/
	void LoadSettingsFromFileStorage(void);

	//std::vector<cv::Rect> faces_;

    GlobalFeaturePool   globalFeaturePool_;	    //!< Stores all global feature extractor.   
    LocalFeaturePool    localFeaturePool_;      //!< Stores all local feature extractor.
    HaarDetectorPool    haarDetectorPool_;

    int cameraId_;				//!< ID of the camera to be analyzed.

    MotionHistory* motionHistory_;
	cv::VideoCapture videoCapture_;
	cv::Mat frame_;
};
