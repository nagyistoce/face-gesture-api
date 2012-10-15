#include "MotionHistory.h"
#include "Definitions.h"

#include <ctime>

using namespace std;
using namespace cv;

MotionHistory::MotionHistory(const Size& size)
:   lastId_(0)
{
    buffer_ = new Mat[BUFFER_SIZE];
    for(int i = 0; i < BUFFER_SIZE; i++)
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

void MotionHistory::UpdateMotionHistory(const Mat& image, int diffThreshold)
{
    // get current time in seconds
    double timestamp = (double)clock() / CLOCKS_PER_SEC;
    int idx1 = lastId_;

    // convert frame to gray scale
    cvtColor(image, buffer_[lastId_], CV_BGR2GRAY); 

    // index of (last - (BUFFER_SIZE-1))th frame
    int idx2 = (lastId_ + 1) % BUFFER_SIZE;
    lastId_ = idx2;

    // get difference between frames
    Mat silh = buffer_[idx2];
    absdiff(buffer_[idx1], buffer_[idx2], silh);

    // and threshold it
    threshold(silh, silh, diffThreshold, 1, CV_THRESH_BINARY);

    // update MHI
    updateMotionHistory(silh, mhi_, timestamp, MHI_DURATION);

    // convert MHI to blue 8u image
    mhi_.convertTo(mask_, mask_.type(), 255.0 / MHI_DURATION, (MHI_DURATION - timestamp) * 255.0 / MHI_DURATION);
}

const Mat& MotionHistory::GetMhi(void)
{
    return mhi_;
}

const Mat& MotionHistory::GetMask(void)
{
    return mask_;
}
