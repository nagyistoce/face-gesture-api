#include <ctime>
#include <cmath>

#include "MotionHistory.h"
#include "Definitions.h"
#include "Visualizer.h"
#include "PointTracker.h"
#include <iomanip>

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
    procFPS_(0.0)
{
    buffer_ = new Mat[bufferSize_];
    for(int i = 0; i < bufferSize_; i++)
        buffer_[i] = Mat::zeros(size, CV_8UC1);

    mhi_ =         Mat::zeros(size, CV_32FC1);
	orientation_ = Mat::zeros(size, CV_32FC1);
    motionMask_ =        Mat::zeros(size, CV_8UC1);

	motionPicture_ = Mat::zeros(size, CV_8UC3);

	magnitude_ = Mat::zeros(size, CV_64F);
	angle_ = Mat::zeros(size, CV_64F);
	gradX = Mat::zeros(size, CV_64F);
	gradY = Mat::zeros(size, CV_64F);

	for(int i = 0; i < 3; i++)
	{
		splitChannels_.push_back(Mat(size, CV_8UC1, Scalar(255)));
	}
}

MotionHistory::~MotionHistory(void)
{
    delete[] buffer_; buffer_ = NULL;
	splitChannels_.clear();
}

void MotionHistory::UpdateMotionHistory(const Mat& image)
{
	stopwatch_.Reset();
    //procTime_ = (double)cvGetTickCount();

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
    mhi_.convertTo(motionMask_, motionMask_.type(), 255.0 / mhiDuration_, (mhiDuration_ - timestamp) * 255.0 / mhiDuration_);

	Sobel(motionMask_, gradX, magnitude_.type(), 1, 0, 3, 1, 0, BORDER_DEFAULT);
	Sobel(motionMask_, gradY, magnitude_.type(), 0, 1, 3, 1, 0, BORDER_DEFAULT);
	phase(gradX, gradY, angle_, false);
	magnitude(gradX, gradY, magnitude_);
	cv::normalize(magnitude_, magnitude_, 0.0, 1.0, cv::NORM_MINMAX);

	motionPicture_ = Mat::zeros(motionMask_.size(), CV_8UC3);
	splitChannels_[1] = 255 - motionMask_;
	splitChannels_[2] = 255 - motionMask_;
	cv::merge(splitChannels_, motionPicture_);

    procFPS_ = stopwatch_.GetFPS();
}

const Mat& MotionHistory::GetMhi(void)
{
    return mhi_;
}

const Mat& MotionHistory::GetMotionMask(void)
{
    return motionMask_;
}

const cv::Mat& MotionHistory::GetMotionPicture() const
{
	return motionPicture_;
}

double MotionHistory::GetWeightedAngle(Mat& mag, Mat& ang)
{
	double res = 0;
	double n = 0;

	for(int i = 0; i < mag.rows; ++i)
	{
		for(int j = 0; j < mag.cols; ++j)
		{
			res += ang.at<double>(i, j) * mag.at<double>(i, j);
			n += mag.at<double>(i, j);
		}
	}

	return std::fabs(n) > DBL_EPSILON ? res / n : res;
}

void MotionHistory::Visualize(void)
{
    Mat outputImg(mhi_.rows, mhi_.cols * 2, CV_8UC3, Scalar(0));
    Mat& window1 = outputImg(Rect(0, 0, mhi_.cols, mhi_.rows));
    Mat& window2 = outputImg(Rect(mhi_.cols, 0, mhi_.cols, mhi_.rows));

	cvtColor(255.0 - motionMask_, window1, CV_GRAY2RGB);

    equalizeHist(silh_, silh_);
    motionPicture_.copyTo(window2);

	int blockSize = window1.cols / 25 - 1;
	double r = static_cast<double>(blockSize);

	for(int i = 0; i < window1.rows - blockSize; i+= blockSize)
	{
		for(int j = 0; j < window1.cols - blockSize; j+= blockSize)
		{
			double a = GetWeightedAngle(magnitude_(Rect(j,i,blockSize,blockSize)), angle_(Rect(j,i,blockSize,blockSize)));

			double dx = r * cos(a);
			double dy = r * sin(a);
			int x = j;
			int y = i;            

			cv::line(window1, cv::Point(x, y), cv::Point(cvRound(x + dx), cvRound(y + dy)), Scalar(0, 255, 0), 1, CV_AA);
		}
	}

    line(outputImg, Point(mhi_.cols, 0), Point(mhi_.cols, mhi_.rows), Scalar::all(255), 1, CV_AA);

    stringstream ss;
    ss << "MHI: " << cvRound(procFPS_) << " FPS.";

    //VisualizerPtr->PutText(outputImg, ss.str(), Point(10, 20));
	VisualizerPtr->ShowImage("Spatio-temporal Movement Segmentation: Mask & MHI", outputImg);
}
