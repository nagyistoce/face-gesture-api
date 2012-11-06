#pragma once
#pragma warning(disable: 4251)

#include "opencv2/opencv.hpp"

#include "Thread.h"

extern "C++" class __declspec(dllexport) HaarDetector : public Thread
{
public:
	HaarDetector(const std::string& name);
	~HaarDetector(void);

    //! Connects a frame to the feature extractor.
	/*!
		\param frame Output parameter for the current frame.
	*/
	void SetFrame(const cv::Mat& frame, const cv::Mat& prevFrame);

	void Process(void);
	const std::vector<cv::Rect>& GetObjects(void);
	const std::string& GetName(void);

	void Visualize(void);

private:
	//! Implemented virtual method for loading algorithm specific settings from the given storage.
	/*!
		\sa Feature::LoadSettingsFromFileStorage()
	*/
	void LoadSettingsFromFileStorage(void);

	//! Implemented virtual method for the algorithm.
	/*!
		\sa Thread::Run()
	*/
	void* Run(void);

    cv::Mat frame_;		//!< The current frame.
    cv::Mat prevFrame_;		//!< The current frame.

	cv::CascadeClassifier cascade_;
	std::string cascadeName_;
	const std::string name_;
	std::vector<cv::Rect> objects_;

    bool opticalFlow_;
	double scaleFactor_;
	int minNeighbors_;
	int flags_;
	cv::Size minSize_;
	cv::Size maxSize_;
	double procTime_;
};
