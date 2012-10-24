#include <ctime>
#include <cmath>

#include "MotionHistory.h"
#include "Definitions.h"
#include "Visualizer.h"
#include "PointTracker.h"

using namespace std;
using namespace cv;

MotionHistory::MotionHistory(const Size& size, const int bufferSize, const int mhiDuration, const double maxTimeDelta, const double minTimeDelta, const int diffThreshold)
:   buffer_(NULL),
    lastId_(0),
    bufferSize_(bufferSize),
    mhiDuration_(mhiDuration),
    maxTimeDelta_(maxTimeDelta),
    minTimeDelta_(minTimeDelta),
    diffThreshold_(diffThreshold)
{
    buffer_ = new Mat[bufferSize_];
    for(int i = 0; i < bufferSize_; i++)
        buffer_[i] = Mat::zeros(size, CV_8UC1);

    mhi_ =      Mat::zeros(size, CV_32FC1);
    orient_ =   Mat::zeros(size, CV_32FC1);
    segmask_ =  Mat::zeros(size, CV_32FC1);
    mask_ =     Mat::zeros(size, CV_8UC1);
}


MotionHistory::~MotionHistory(void)
{
    delete[] buffer_;
}

void MotionHistory::UpdateMotionHistory(const Mat& image)
{
    // get current time in seconds
    double timestamp = (double)clock() / CLOCKS_PER_SEC;
    int idx1 = lastId_;

    // convert frame to gray scale
    cvtColor(image, buffer_[lastId_], CV_BGR2GRAY); 

    // index of (last - (BUFFER_SIZE-1))th frame
    int idx2 = (lastId_ + 1) % bufferSize_;
    lastId_ = idx2;

    // get difference between frames
    Mat silh = buffer_[idx2];
    absdiff(buffer_[idx1], buffer_[idx2], silh);

    // and threshold it
    threshold(silh, silh, diffThreshold_, 1, CV_THRESH_BINARY);

    // update MHI
    updateMotionHistory(silh, mhi_, timestamp, mhiDuration_);

    // convert MHI to blue 8u image
    mhi_.convertTo(mask_, mask_.type(), 255.0 / mhiDuration_, (mhiDuration_ - timestamp) * 255.0 / mhiDuration_);
}

const Mat& MotionHistory::GetMhi(void)
{
    return mhi_;
}

const Mat& MotionHistory::GetMask(void)
{
    return mask_;
}

void MotionHistory::Visualize(void)
{
	//Mat maskCopy(frame_.rows, frame_.cols, CV_8UC3);
	//cvtColor(motionHistory_->GetMask(), maskCopy, CV_GRAY2BGR);
	//addWeighted(maskCopy, 0.5, frame_, 1.0, 0.0, frame_);

	VisualizerPtr->ShowImage("UpdateMotionHistory - MHI", mhi_);
	VisualizerPtr->ShowImage("UpdateMotionHistory - Mask", mask_);
}
