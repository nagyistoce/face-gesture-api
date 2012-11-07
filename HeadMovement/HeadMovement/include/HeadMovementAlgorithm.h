#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"

class HaarDetector;
class MotionHistory;
class GlobalFeature;
class LocalFeature;
class ImageFrame;
class Thread;
class PointTracker;

typedef std::map<std::string, GlobalFeature*> GlobalFeaturePool;
typedef std::map<std::string, LocalFeature*> LocalFeaturePool;
typedef std::map<std::string, HaarDetector*> HaarDetectorPool;
typedef std::map<std::string, Thread*> ThreadPool;

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
    void StartFeatureExtractors(void);
    void StartDetectors( void );
    void VisualizeProcesses( void );
    std::vector<cv::Rect> faces_;

	ThreadPool			threadPool_;
    GlobalFeaturePool   globalFeaturePool_;	    //!< Stores all global feature extractor.   
    LocalFeaturePool    localFeaturePool_;      //!< Stores all local feature extractor.
    HaarDetectorPool    haarDetectorPool_;

    int cameraId_;				//!< ID of the camera to be analyzed.
	std::string fileName_;
    cv::Size resolution_;
    std::vector<cv::KeyPoint> keyPoints_;
	std::vector<double> angles_;
    bool motionStarted_;
    bool motionEnded_;

    MotionHistory* motionHistory_;
    PointTracker* pointTracker_;
	cv::VideoCapture videoCapture_;
	cv::Mat frame_;
    cv::Mat prevFrame_;
};
