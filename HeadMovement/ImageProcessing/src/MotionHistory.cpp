#include <ctime>
#include <cmath>

#include "MotionHistory.h"
#include "Definitions.h"
#include "Visualizer.h"
#include "PointTracker.h"

using namespace std;
using namespace cv;

MotionHistory::MotionHistory(const Size& size, const int bufferSize, const double mhiDuration, const double maxTimeDelta, const double minTimeDelta, const int diffThreshold)
:   buffer_(NULL),
    lastId_(0),
    bufferSize_(bufferSize),
    mhiDuration_(mhiDuration),
    maxTimeDelta_(maxTimeDelta),
    minTimeDelta_(minTimeDelta),
    diffThreshold_(diffThreshold),
    procTime_(0.0)
{
    buffer_ = new Mat[bufferSize_];
    for(int i = 0; i < bufferSize_; i++)
        buffer_[i] = Mat::zeros(size, CV_8UC1);

    mhi_ =      Mat::zeros(size, CV_32FC1);
    mask_ =     Mat::zeros(size, CV_8UC1);
}


MotionHistory::~MotionHistory(void)
{
    delete[] buffer_;
}

void MotionHistory::UpdateMotionHistory(const Mat& image)
{
    procTime_ = (double)cvGetTickCount();

    // get current time in seconds
    double timestamp = (double)clock() / CLOCKS_PER_SEC;
    int idx1 = lastId_;

    // convert frame to gray scale
    cvtColor(image, buffer_[lastId_], CV_BGR2GRAY); 

    // index of (last - (BUFFER_SIZE-1))th frame
    int idx2 = (lastId_ + 1) % bufferSize_;
    lastId_ = idx2;

    // get difference between frames
    absdiff(buffer_[idx1], buffer_[idx2], silh_);

    // and threshold it
    threshold(silh_, silh_, diffThreshold_, 1, CV_THRESH_BINARY);

    // update MHI
    updateMotionHistory(silh_, mhi_, timestamp, mhiDuration_);

    // convert MHI to blue 8u image
    mhi_.convertTo(mask_, mask_.type(), 255.0 / mhiDuration_, (mhiDuration_ - timestamp) * 255.0 / mhiDuration_);

    procTime_ = (double)cvGetTickCount() - procTime_;
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
    Mat outputImg(mhi_.rows, mhi_.cols * 2, CV_8UC1, Scalar(0));
    Mat& window1 = outputImg(Rect(0, 0, mhi_.cols, mhi_.rows));
    Mat& window2 = outputImg(Rect(mhi_.cols, 0, mhi_.cols, mhi_.rows));

    equalizeHist(silh_, silh_);
    silh_.copyTo(window1);
    mask_.copyTo(window2);
    line(outputImg, Point(mhi_.cols, 0), Point(mhi_.cols, mhi_.rows), Scalar(255));

    stringstream ss;
    ss << "Processing time: " << procTime_ / ((double)cvGetTickFrequency() * 1000.0) << " ms.";

    VisualizerPtr->PutText(outputImg, ss.str(), Point(10, 20));
	VisualizerPtr->ShowImage("UpdateMotionHistory: Mask & MHI", outputImg);
}
