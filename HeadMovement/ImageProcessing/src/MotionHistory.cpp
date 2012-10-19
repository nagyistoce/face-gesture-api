#include <ctime>
#include <cmath>

#include "MotionHistory.h"
#include "Definitions.h"
#include "Visualizer.h"
#include "PointTracker.h"

using namespace std;
using namespace cv;

MotionHistory::MotionHistory(const Size& size)
:   lastId_(0),
    pointTracker_(NULL)
{
    buffer_ = new Mat[BUFFER_SIZE];
    for(int i = 0; i < BUFFER_SIZE; i++)
        buffer_[i] = Mat::zeros(size, CV_8UC1);

    mhi_ =      Mat::zeros(size, CV_32FC1);
    orient_ =   Mat::zeros(size, CV_32FC1);
    segmask_ =  Mat::zeros(size, CV_32FC1);
    mask_ =     Mat::zeros(size, CV_8UC1);

    pointTracker_ = new PointTracker("LK");
}


MotionHistory::~MotionHistory(void)
{
    delete[] buffer_;
    delete pointTracker_;
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

void MotionHistory::PredictMotionVectors(const Mat& frame, const Mat& prevFrame, const Rect& rect, const vector<Point2f>& points)
{
    Point2f center(rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f);
    double radius = (rect.width + rect.height) * 0.25f;
    vector<Point2f> innerPoints, nextPoints;

    vector<uchar> status;
    vector<float> err;

    double procTime = (double)cvGetTickCount();

    for(size_t i = 0; i < points.size(); i++)
    {
        double dst = sqrt(pow(center.x - points[i].x, 2) + pow(center.y - points[i].y, 2));

        if(dst < radius)
            innerPoints.push_back(points[i]);
    }

    Mat grayFrame, grayPrevFrame;

    if(!innerPoints.empty())
    {      
        if(!frame.empty() && !prevFrame.empty())
        {
            grayFrame = frame.clone();
            if(grayFrame.channels() != 1)
                cvtColor(grayFrame, grayFrame, CV_BGR2GRAY);

            grayPrevFrame = prevFrame.clone();
            if(grayPrevFrame.channels() != 1)
                cvtColor(grayPrevFrame, grayPrevFrame, CV_BGR2GRAY);

            TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
            calcOpticalFlowPyrLK(grayPrevFrame, grayFrame, innerPoints, nextPoints, status, err, Size(31,31),
                                 3, termcrit, 0, 0.001);

            if(innerPoints.size() == nextPoints.size())
                for(size_t i = 0; i < nextPoints.size(); i++ )
                {
                    if( !status[i] )
                        continue;
                
                    line(grayFrame, Point(innerPoints[i]), Point(nextPoints[i]), Scalar(255, 255, 255), 1);
                    circle(grayFrame, innerPoints[i], 2, Scalar(255, 255, 255), -1);
                }
        }
    }

    procTime = (double)cvGetTickCount() - procTime;

    if(!grayFrame.empty())
    {
        stringstream ss;

        ss << "Processing time of MotionHistory: " << procTime / (cvGetTickFrequency() * 1000.0);
        VisualizerPtr->PutText(grayFrame, ss.str(), Point(10, 20));
        ss.str("");

        VisualizerPtr->ShowImage("MotionHistory", grayFrame);
    }
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
