#include "dolslr/doldef.h"
#include "dolslr/Contour.h"
#include "opencv2/legacy/legacy.hpp"

using namespace std;
using namespace cv;

Contour::Contour(void)
{
}

Contour::~Contour(void)
{
}

void Contour::InitProcess(Mat& frame)
{
	if(frame.channels() == 3)
		cvtColor(frame, grayFrame_, CV_BGR2GRAY);
	else
		grayFrame_ = frame.clone();

	//resize(grayFrame_, grayFrame_, size_);
}

void Contour::FindLargestContour(Mat& frame, vector<Point>& handContour)
{
	double maxSize = -1;
	int maxInd = -1;

	InitProcess(frame);

	// Find all of the contours.
	findContours(grayFrame_, contours_, hierarchy_, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);

	// Find the biggest contour over the current frame.
	for( size_t i = 0; i < contours_.size(); i++ )
	{
		double actSize = contourArea(contours_[i]);

		if(actSize > maxSize)
		{
			maxSize = actSize;
			handContour = contours_[i];
			maxInd = i;
		}
	}
}

void Contour::ChainCode(vector<Point>& contour, vector<int>& chainCode)
{
	int length = contour.size();

	for(int i = 0; i < length; i++)
	{
		int j = ( i + 1 ) % length;

		if( contour[i].x == contour[j].x && contour[i].y > contour[j].y )
			chainCode.push_back(2);
		else if( contour[i].x < contour[j].x && contour[i].y > contour[j].y )
			chainCode.push_back(1);
		else if( contour[i].x < contour[j].x && contour[i].y == contour[j].y )
			chainCode.push_back(0);
		else if( contour[i].x < contour[j].x && contour[i].y < contour[j].y )
			chainCode.push_back(7);
		else if( contour[i].x == contour[j].x && contour[i].y < contour[j].y )
			chainCode.push_back(6);
		else if( contour[i].x > contour[j].x && contour[i].y < contour[j].y )
			chainCode.push_back(5);
		else if( contour[i].x > contour[j].x && contour[i].y == contour[j].y )
			chainCode.push_back(4);
		else
			chainCode.push_back(3);
	}
}

void Contour::SmoothChainCode(vector<int>& chainCode, vector<double>& chainCodeSmoothed)
{
	int length = chainCode.size();

	// v_new(n) = (1/16)*(v(n-2) + 4* v(n-1) + 6* v(n) + 4*(v(n+1) + v(n+2))
	for(int i = 0; i < length; i++)
	{
		int ip1 = ( i + 1 ) % length;
		int ip2 = ( i + 2 ) % length;
		int im1 = i == 0 ? length - 1 : i - 1;
		int im2 = i == 0 ? length - 2 : ( i == 1 ? length - 1 : i - 2 );

		chainCodeSmoothed.push_back( 1.0/16.0 * (chainCode[im2] + 4.0*chainCode[im1] + 6.0*chainCode[i] + 4.0*chainCode[ip1] + chainCode[ip2]) );
	}
}

void Contour::DeriveChainCode(vector<double>& chainCode, vector<double>& chainCodeFirstDiff, vector<double>& chainCodeSecondDiff)
{
	int length = chainCode.size();

	for (int i = 0; i < length; i++) 
	{
		int ip1 = ( i + 1 ) % length;
		int im1 = i == 0 ? length - 1 : i - 1;

		chainCodeFirstDiff.push_back(chainCode[ip1] - chainCode[im1]);
		chainCodeSecondDiff.push_back(chainCode[ip1] - 2 * chainCode[i] + chainCode[im1]);
	}
}

void Contour::DrawContour(Mat& image, vector<Point>& contour, Scalar& color, int thickness )
{
	//int length = contour.size();
	////int r = 0, g = 0, b = 0;

	//for(int i = 0; i < length; i++)
	//{
	//	//Scalar color = Scalar(r % 255, b % 255, g % 255);
	//	int j = ( i + 1 ) % length;

	//	line(image, contour[i], contour[j], color, thickness);
	//	//r += 2, g += 2, b += 2;
	//}

	vector<vector<Point> > contoursArray;

	contoursArray.push_back(contour);

	drawContours(image, contoursArray, 0, color, thickness);
}

void Contour::DrawContour2f(Mat& image, vector<Point2f>& contour, Scalar& color, int thickness )
{
	vector<vector<Point2f> > contoursArray;

	contoursArray.push_back(contour);

	drawContours(image, contoursArray, 0, color, thickness);
}

void Contour::ILocateConvexityDefects(vector<Point>& contour, vector<int>& hull, vector<Point>& startPoint, vector<Point>& endPoint, vector<Point>& depthPoint, vector<float>& depth)
{
	if(hull.size() > 0 && contour.size() > 0)
	{
		CvSeq* contourPoints;
		CvSeq* defects;
		CvMemStorage* storage;
		CvMemStorage* strDefects;
		CvMemStorage* contourStr;
		CvConvexityDefect *defectArray = 0;

		startPoint.clear();
		endPoint.clear();
		depthPoint.clear();
		depth.clear();

		strDefects = cvCreateMemStorage();
		defects = cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvSeq),sizeof(CvPoint), strDefects );

		// Transform our vector<Point> into a CvSeq* object of CvPoint.
		contourStr = cvCreateMemStorage();
		contourPoints = cvCreateSeq(CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), contourStr);

		for(int i = 0; i < (int)contour.size(); i++) 
		{
			CvPoint cp = {contour[i].x, contour[i].y};
			cvSeqPush(contourPoints, &cp);
		}

		// Now, we do the same thing with the hull index
		int count = (int)hull.size();
		// int hullK[count];
		int* hullK = (int*)malloc(count * sizeof(int));
		for(int i = 0; i < count; i++)
			hullK[i] = hull.at(i);

		CvMat hullMat = cvMat(1, count, CV_32SC1, hullK);

		// Calculate convexity defects
		storage = cvCreateMemStorage(0);
		defects = cvConvexityDefects(contourPoints, &hullMat, storage);
		defectArray = (CvConvexityDefect*)malloc(defects->total * sizeof(CvConvexityDefect));
		cvCvtSeqToArray(defects, defectArray, CV_WHOLE_SEQ);
		//printf("DefectArray %i %i\n",defectArray->end->x, defectArray->end->y);

		// Store defects points in the convexDefects parameter.
		for(int i = 0; i < defects->total; i++)
		{
			CvPoint ptf;
			ptf.x = defectArray[i].start->x;
			ptf.y = defectArray[i].start->y;
			startPoint.push_back(ptf);

			ptf.x = defectArray[i].end->x;
			ptf.y = defectArray[i].end->y;
			endPoint.push_back(ptf);

			ptf.x = defectArray[i].depth_point->x;
			ptf.y = defectArray[i].depth_point->y;
			depthPoint.push_back(ptf);

			depth.push_back(defectArray[i].depth);
		}

		// Release memory
		cvReleaseMemStorage(&contourStr);
		cvReleaseMemStorage(&strDefects);
		cvReleaseMemStorage(&storage);

		free(hullK);
		free(defectArray);
	}
}

void Contour::ISnakeImage(Mat& contourMask, vector<Point>& activeContour)
{
	IplImage *iplContourMask = new IplImage(contourMask);
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contours = 0;

	cvThreshold(iplContourMask, iplContourMask, 141, 255, CV_THRESH_BINARY); 
	cvFindContours(iplContourMask, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	if(!contours) 
	{
		delete iplContourMask;
		cvReleaseMemStorage(&storage);
		return;
	}

	int length = contours->total;

	if(length < 10)
	{
		delete iplContourMask;
		cvReleaseMemStorage(&storage);
		return;
	}

	CvPoint* points = new CvPoint[length]; 

	CvSeqReader reader;
	CvPoint pt = cvPoint(0, 0);	
	CvSeq* contour2 = contours;	

	cvStartReadSeq(contour2, &reader);
	for(int i = 0; i < length; i++)
	{
		CV_READ_SEQ_ELEM(pt, reader);
		points[i] = pt;
	}

	cvReleaseMemStorage(&storage);

	float alpha = snakeParam_.ialpha / 100.0f; 
	float beta = snakeParam_.ibeta / 100.0f; 
	float gamma = snakeParam_.igamma / 100.0f; 

	cvSnakeImage(iplContourMask, points, length, &alpha, &beta, &gamma, snakeParam_.coeff_usage, snakeParam_.size, snakeParam_.criteria, snakeParam_.calc_gradient);

	for(int i = 0; i < length; i++)
		activeContour.push_back(Point(points[i].x, points[i].y));

	delete iplContourMask;
	delete[] points;
}


void Contour::SetSnakeParameters(int alpha, int beta, int gamma, double perimeter, Size size, int maxIter, double eps)
{
	snakeParam_.ialpha = alpha;
	snakeParam_.ibeta = beta;
	snakeParam_.igamma = gamma;

	snakeParam_.perimeter = perimeter;

	// Different uses of the previous three parameters:
	// - CV_VALUE - indicates that each of alpha, beta, gamma is a pointer to a single value to be used for all points;
	// - CV_ARRAY - indicates that each of alpha, beta, gamma is a pointer to an array of coefficients different for all the points of the snake. All the arrays must have the size equal to the contour size.
	snakeParam_.coeff_usage = CV_VALUE;

	snakeParam_.size.width = size.width; 
	snakeParam_.size.height = size.height; 

	snakeParam_.criteria.type = CV_TERMCRIT_ITER; 
	snakeParam_.criteria.max_iter = maxIter; 
	snakeParam_.criteria.epsilon = eps; 

	snakeParam_.calc_gradient = 0;
}
