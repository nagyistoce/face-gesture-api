#include "HeadMovementAlgorithm.h"
#include "Definitions.h"
#include "HaarDetector.h"
#include "MotionHistory.h"

using namespace std;
using namespace cv;

HeadMovementAlgorithm::HeadMovementAlgorithm(void)
:   motionHistory_(NULL)
{
	videoCapture_.open(0);

	// check if we succeeded
	if(!videoCapture_.isOpened())
		CV_Error(1, "VideoCapture is not opened!");

	faceDetector_ = new HaarDetector("FaceDetector");
}

HeadMovementAlgorithm::~HeadMovementAlgorithm(void)
{
	delete faceDetector_;
    delete motionHistory_;
}

void HeadMovementAlgorithm::ProcessAlgorithm(void)
{
	cout << "HeadMovementAlgorithm: Press ESC to exit." << endl;

	while(true)
	{
#ifdef DEBUG_MODE
        double t = (double)cvGetTickCount();
#endif

		// get a new frame from camera
		videoCapture_ >> frame_;

		// detects faces on the incoming frames
		faces_ = faceDetector_->Detect(frame_);

		if(prevFrame_.empty() == false)
        {
			absdiff(frame_, prevFrame_, subFrame_);
            imshow("HeadMovementAlgorithm - SubFrame", subFrame_);
        }

        if(motionHistory_ == NULL)
            motionHistory_ = new MotionHistory(frame_.size());

        motionHistory_->UpdateMotionHistory(frame_, 30);
        prevFrame_ = frame_.clone();

        DrawObjects(frame_, faces_);

        Mat maskCopy(frame_.rows, frame_.cols, CV_8UC3);
        cvtColor(motionHistory_->GetMask(), maskCopy, CV_GRAY2BGR);
        addWeighted(maskCopy, 0.5, frame_, 1.0, 0.0, frame_);

        imshow("HeadMovementAlgorithm - Frame", frame_);
        imshow("UpdateMotionHistory - MHI", motionHistory_->GetMhi());
        imshow("UpdateMotionHistory - Mask", motionHistory_->GetMask());
        
		// press ESC to exit
		if(waitKey(30) >= 0) 
			break;

#ifdef DEBUG_MODE
        t = (double)cvGetTickCount() - t;
        printf("Processing time: %g ms.\n", t / ((double)cvGetTickFrequency() * 1000.0));
#endif
	}
}

void HeadMovementAlgorithm::DrawObjects(Mat& image, const vector<Rect>& objects)
{
	int i = 0;
	const static Scalar colors[8] =  { 
		CV_RGB(0 ,0, 255),
		CV_RGB(0, 128, 255),
		CV_RGB(0, 255, 255),
		CV_RGB(0, 255, 0),
		CV_RGB(255, 128, 0),
		CV_RGB(255, 255, 0),
		CV_RGB(255, 0, 0),
		CV_RGB(255, 0, 255)
	};

	for(vector<Rect>::const_iterator r = objects.begin(); r != objects.end(); r++, i++)
	{
		Point center(cvRound(r->x + r->width * 0.5), cvRound(r->y + r->height * 0.5));
		int radius = cvRound((r->width + r->height) * 0.25);
		circle(image, center, radius, colors[i % 8], 3);
	}
}
