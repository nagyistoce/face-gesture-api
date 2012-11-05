#include "dolslr/doldef.h"
#include "dolslr/HandContour.h"

#include "dolslr/Visualizer.h"
#include "dolslr/GraphUtils.h"
#include "dolslr/Contour.h"

#include "dolslr/MatWindow.h"
#include "dolslr/WindowGroupButton.h"

using namespace std;
using namespace cv;

HandContour::HandContour(WindowGroupButton* button)
{
	window_ = new MatWindow("HandContour - ChainCode");
	button->AddWindow(window_);
}

HandContour::HandContour(WindowGroupButton* button, const string& name)
:	name_(name)
{
	window_ = new MatWindow(string("HandContour - ChainConde - ") + name);
	button->AddWindow(window_);

	contour_ = new Contour();
	chainPlotter_ = new GraphUtils(Size(640, 480), true, false);
}

HandContour::~HandContour(void)
{
	delete contour_;
	delete chainPlotter_;
}

void HandContour::ActiveContourFitting(Mat& wholeFrameRoi, Mat& handMask)
{
	vector<Point> largestHandContour;

	activeContour.clear();

	output_ = Mat::zeros(wholeFrameRoi.rows, wholeFrameRoi.cols, CV_8UC3);
	contour_->FindLargestContour(handMask, largestHandContour);

	// Analyze the hand based on the biggest contour
	if (!largestHandContour.empty())
	{
		Mat contourMask = Mat::zeros(wholeFrameRoi.rows, wholeFrameRoi.cols, CV_8UC1);
		vector<Point> approxContour, hullForDrawing;

		contour_->DrawContour(contourMask, largestHandContour, Scalar(255.0), -1);
		contour_->SetSnakeParameters(100, 100, 100, arcLength(largestHandContour, true), Size(3, 3), 1000, 0.0025);
		contour_->ISnakeImage(contourMask, activeContour);

		if(activeContour.empty()) return;

		// Approximate the activeContour with a polygon.
		approxPolyDP(activeContour, approxContour, arcLength(activeContour, true) * 0.004, true);

		// Get and draw the convex hull, of the biggest contour.
		convexHull( approxContour, hullForDrawing, false, false );	// Convex hull points for drawing.
		convexHull( approxContour, hull, false, true );				// Indices of the convex hull points for convexity defects.

		// The bounding rectangle of biggest contour. It is not parallel with the main axises.
		minRect = minAreaRect( approxContour ); 

		// Locate the convexity defects.
		contour_->ILocateConvexityDefects(approxContour, hull, startPoint_, endPoint_, depthPoint_, depth_);
		DrawAndComputeFingersNum(wholeFrameRoi);
		DrawContours(wholeFrameRoi, largestHandContour, hullForDrawing);	
	}

	//resize(output_, output_, Size(150, cvRound(output_.rows * (150.0 / output_.cols))));
	//output_.copyTo( VisualizerPtr->ShowFrame("AnalyzeActiveContour - "  + name_) );
}

void HandContour::ActiveContourStandardizing(Mat& fullFrame, Rect& handRoi, Rect& elbowRoi)
{
	if (!activeContour.empty())
	{
		Point pt1 = Point(cvRound(minRect.center.x + handRoi.x), cvRound(minRect.center.y + handRoi.y));
		Point pt2 = Point(cvRound(elbowRoi.x + elbowRoi.width / 2.0), cvRound(elbowRoi.y + elbowRoi.height / 2.0));

		circle(fullFrame, pt2, 3, Scalar(200.0, 125.0, 75.0), -2 );
		line(fullFrame, pt1, pt2, Scalar(255.0, 0.0, 0.0));

		double minDst = DBL_MAX;
		int minInd = -1;

		// Gets pixels along the line (pt1, pt2)
		LineIterator it(fullFrame, pt1, pt2, 8);
		for(int i = 0; i < it.count; i++, ++it)
		{
			Point pt3 = it.pos();
			//circle(fullFrame, pt3, 3, Scalar(200.0, 200.0, 100.0), -2 );

			for(int j = 0; j < static_cast<int>(activeContour.size()); j++)
			{
				Point pt4 = Point(activeContour[j].x + handRoi.x, activeContour[j].y + handRoi.y);
				double dst = sqrt(pow(pt3.x - pt4.x, 2.0) + pow(pt3.y - pt4.y, 2.0));

				if(dst < minDst)
					minDst = dst, minInd = j;
			}
		}

		if(minInd != -1)
		{
			Point pt4 = Point(activeContour[minInd].x + handRoi.x, activeContour[minInd].y + handRoi.y);

			circle(fullFrame, pt4, 3, Scalar(0, 0, 255), -2 );
			rotate(activeContour.begin(), activeContour.begin() + minInd, activeContour.end());
		}
	}

	//fullFrame.copyTo( VisualizerPtr->ShowFrame("Hand Contours") );
}

void HandContour::ShowChainGraphs(bool showChain = true, bool showSmoothed = true, bool showFirstDiff = true, bool showSecondDiff = true)
{
	Mat imageGraph;

	if(showChain) 
		chainPlotter_->DrawIntGraph(chainCode, imageGraph, Scalar(0.0, 0.0, 255.0), ( "Chain Code " + name_ ).c_str(), -10, 10);

	if(showSmoothed) 
		chainPlotter_->DrawFloatGraph(chainCodeSmoothed, imageGraph, Scalar(255.0, 0.0, 0.0), ( "Chain Code (smoothed) " + name_ ).c_str(), -10, 10);

	if(showFirstDiff) 
		chainPlotter_->DrawFloatGraph(chainCodeFirstDiff, imageGraph, Scalar(0.0, 255.0, 0.0), ( "First Derivate of Chain Code (smoothed) " + name_ ).c_str(), -10, 10);

	if(showSecondDiff) 
		chainPlotter_->DrawFloatGraph(chainCodeSecondDiff, imageGraph, Scalar(255.0, 0.0, 255.0), ( "Second Derivate of Chain Code (smoothed) " + name_ ).c_str(), -10, 10);

	if(!imageGraph.empty())
		window_->SetImage(imageGraph);
}

void HandContour::MirrorChains(void)
{
	if(chainCode.empty() || chainCodeSmoothed.empty() || chainCodeFirstDiff.empty())
		return;

	int n = chainCodeFirstDiff.size();
	int h = n / 2;
	int tmp;

	for(int i = 0; i < h; i++) 
	{
		int j = (n - i) - 1;

		tmp = chainCode[j];
		chainCode[j] = chainCode[i];
		chainCode[i] = tmp;

		tmp = chainCodeSmoothed[j];
		chainCodeSmoothed[j] = chainCodeSmoothed[i];
		chainCodeSmoothed[i] = tmp;

		tmp = chainCodeFirstDiff[j];
		chainCodeFirstDiff[j] = chainCodeFirstDiff[i];
		chainCodeFirstDiff[i] = tmp;
	}
}

void HandContour::GetAndSmoothAndDeriveChain(void)
{
	chainCode.clear(), chainCodeSmoothed.clear(), chainCodeFirstDiff.clear(), chainCodeSecondDiff.clear();

	if(!activeContour.empty())
	{
		contour_->ChainCode(activeContour, chainCode);
		contour_->SmoothChainCode(chainCode, chainCodeSmoothed);	
		contour_->DeriveChainCode(chainCodeSmoothed, chainCodeFirstDiff, chainCodeSecondDiff);		
	}
}

void HandContour::DrawContours(Mat& wholeFrameRoi, vector<Point>& handContour, vector<Point>& hull)
{
	// Draw the polygon approximated contour, the convex hull and the center of convex hull.
	contour_->DrawContour(output_, handContour, Scalar(75.0, 125.0, 200.0), 2);
	contour_->DrawContour(output_, activeContour, Scalar(0.0, 255.0, 0.0), 2);
	contour_->DrawContour(output_, hull, Scalar(125.0, 125.0, 0.0), 1);
	circle(output_, Point(cvRound(minRect.center.x), cvRound(minRect.center.y)), 3, Scalar(200.0, 125.0, 75.0), -2 );

	// Drawing to the full frame
	contour_->DrawContour(wholeFrameRoi, handContour, Scalar(75.0, 125.0, 200.0), 2);
	contour_->DrawContour(wholeFrameRoi, activeContour, Scalar(0.0, 255.0, 0.0), 2);
	contour_->DrawContour(wholeFrameRoi, hull, Scalar(125.0, 125.0, 0.0), 1);
	circle(wholeFrameRoi, Point(cvRound(minRect.center.x), cvRound(minRect.center.y)), 3, Scalar(200.0, 125.0, 75.0), -2 );
}

void HandContour::DrawAndComputeFingersNum(Mat& wholeFrameRoi)
{
	fingerNum = 0;

	for( size_t i = 0; i < depthPoint_.size(); i++ )
	{
		// Relationship between the start point and the centre of bounding box.
		bool rStartAndBouningBox = startPoint_[i].y < minRect.center.y;

		// Relationship between the depth point and the centre of bounding box.
		bool rDepthAndBouningBox = depthPoint_[i].y < minRect.center.y;

		// Relationship between the start-, and depth point.
		bool rStartAndDepth = startPoint_[i].y < depthPoint_[i].y;

		// Euclidean distance of the start-, and depth point.
		double startDepthDist = sqrt(pow(startPoint_[i].x - depthPoint_[i].x, 2.0) + pow(startPoint_[i].y - depthPoint_[i].y, 2.0));

		// Minimum distance.
		double minDist = minRect.size.height / 6.5;

		if((rStartAndBouningBox || rDepthAndBouningBox) && rStartAndDepth && (startDepthDist > minDist))
		{
			fingerNum++;
			line(output_, startPoint_[i], depthPoint_[i], Scalar(50, 250, 50), 2);
			line(output_, endPoint_[i], depthPoint_[i], Scalar(147, 20, 255), 2);

			line(wholeFrameRoi, startPoint_[i], depthPoint_[i], Scalar(50, 250, 50), 2);
			line(wholeFrameRoi, endPoint_[i], depthPoint_[i], Scalar(147, 20, 255), 2);
		}

		circle(output_, startPoint_[i], 3, Scalar(0, 0, 255), -2);
		circle(output_, endPoint_[i], 3, Scalar(128, 0, 0), -2);
		circle(output_, depthPoint_[i], 3, Scalar(0, 255, 255), -2);

		circle(wholeFrameRoi, startPoint_[i], 3, Scalar(0, 0, 255), -2);
		circle(wholeFrameRoi, endPoint_[i], 3, Scalar(128, 0, 0), -2);
		circle(wholeFrameRoi, depthPoint_[i], 3, Scalar(0, 255, 255), -2);
	}

	stringstream text;
	text << "Fingers: " << fingerNum;

	putText(output_, text.str(), Point(5, 10), CV_FONT_HERSHEY_COMPLEX, 0.3, Scalar(255.0, 255.0, 255.0), 1);
	putText(wholeFrameRoi, text.str(), Point(5, 10), CV_FONT_HERSHEY_COMPLEX, 0.3, Scalar(255.0, 0.0, 0.0), 1);
}
