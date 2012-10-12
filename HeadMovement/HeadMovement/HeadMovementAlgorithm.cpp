#include "HeadMovementAlgorithm.h"

using namespace std;
using namespace cv;

HeadMovementAlgorithm::HeadMovementAlgorithm(void)
{
	videoCapture_.open(0);

	// check if we succeeded
	if(!videoCapture_.isOpened())
		CV_Error(1, "VideoCapture is not opened!");
}


HeadMovementAlgorithm::~HeadMovementAlgorithm(void)
{
}

void HeadMovementAlgorithm::ProcessAlgorithm(void)
{
	cout << "HeadMovementAlgorithm: Press ESC to exit." << endl;

	while(true)
	{
		// get a new frame from camera
		videoCapture_ >> frame_;

		// displays the new frame
		imshow("HeadMovementAlgorithm", frame_);

		// press ESC to exit
		if(waitKey(30) >= 0) 
			break;
	}
}
