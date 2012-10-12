#include "HeadMovementAlgorithm.h"
#include "HaarDetector.h"

using namespace std;
using namespace cv;

HeadMovementAlgorithm::HeadMovementAlgorithm(void)
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
}

void HeadMovementAlgorithm::ProcessAlgorithm(void)
{
	cout << "HeadMovementAlgorithm: Press ESC to exit." << endl;

	while(true)
	{
		// get a new frame from camera
		videoCapture_ >> frame_;

		// detects faces on the incoming frames
		faces_ = faceDetector_->Detect(frame_);

		if(prevFrame_.empty() == false)
		{
			Mat subFrame;

			absdiff(frame_, prevFrame_, subFrame);
			DrawObjects(frame_, faces_);

			// displays the new frame
			imshow("HeadMovementAlgorithm", subFrame);
		}
		
		prevFrame_ = frame_.clone();

		// press ESC to exit
		if(waitKey(30) >= 0) 
			break;
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
