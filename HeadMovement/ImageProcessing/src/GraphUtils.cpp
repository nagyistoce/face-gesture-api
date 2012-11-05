#include "dolslr/doldef.h"
#include "dolslr/GraphUtils.h"
#include "opencv2/imgproc/imgproc.hpp"

#define BLACK	cv::Scalar(0.0,		0.0,	0.0)
#define WHITE	cv::Scalar(255.0,	255.0,	255.0)
#define GREY	cv::Scalar(150.0,	150.0,	150.0)

using namespace std;
using namespace cv;

GraphUtils::GraphUtils()
{
	graphSize_ = Size(640, 480);
	showScale_ = true;
	showPoints_ = false;
	curvesNo = 0;
}

GraphUtils::GraphUtils(Size graphSize, bool showScale, bool showPoints)
:	graphSize_(graphSize),
	showScale_(showScale),
	showPoints_(showPoints)
{
	curvesNo = 0;
}

GraphUtils::~GraphUtils(void)
{
}

void GraphUtils::DrawFloatGraph(vector<double>& src, Mat& imageGraph, Scalar graphColor, const char* graphLabel, float minV, float maxV)
{
	int w = graphSize_.width;
	int h = graphSize_.height;
	int b = 10;		// border around graph within the image

	int graphElements = src.size();

	if(w <= 20)
		w = graphElements + b * 2;	// width of the image

	if(h <= 20)
		h = 220;

	int s = h - b * 2; // size of graph height
	float xscale = 1.0;

	if(graphElements > 1)
		xscale = (w - b * 2) / (float)(graphElements - 1);	// horizontal scale
	
	if( imageGraph.empty() )
	{
		imageGraph = Mat(h, w, CV_8UC3, WHITE);
		curvesNo = 1;
	}
	else
	{
		curvesNo++;
	}

	// If the user did not supply minV and maxV values, find them from the data, so we can draw it at full scale.
	if(fabs(minV - FLT_MIN) < FLT_EPSILON && fabs(maxV - FLT_MAX) < FLT_EPSILON) 
	{
		for(int i = 0; i < graphElements; i++) 
		{
			float v = (float)src[i];

			if(v < minV)
				minV = v;
			if(v > maxV)
				maxV = v;
		}
	}

	float diffV = maxV - minV;
	if(diffV == 0)
		diffV = 0.00000001f;	// Stop a divide-by-zero error
	float fscale = (float)s / diffV;

	// Draw the horizontal & vertical axis
	int y0 = cvRound(minV * fscale);
	line(imageGraph, Point(b, h - (b - y0)), Point(w - b, h - (b - y0)), BLACK, 2);
	line(imageGraph, Point(b, h - b), Point(b, h - (b + s)), BLACK, 2);

	// Write the scale of the y axis
	if(showScale_ && curvesNo == 1) 
	{
		char text[16];

		// Write the scale of the y axis
		sprintf_s(text, sizeof(text) - 1, "%.1f", maxV);
		putText(imageGraph, text, Point(1, b + 4), CV_FONT_HERSHEY_COMPLEX, 0.5, GREY, 1);

		// Write the scale of the x axis
		sprintf_s(text, sizeof(text) - 1, "%d", (graphElements - 1) );
		putText(imageGraph, text, Point(w - b + 4 - 10 * strlen(text), h - (b - y0) - 4), CV_FONT_HERSHEY_COMPLEX, 0.5, GREY, 1);
	}

	// Draw the values
	Point ptPrev = Point(b, h - (b - y0));	// Start the lines at the 1st point.

	for(int i = 1; i < graphElements; i++) 
	{
		int y = cvRound((src[i] - minV) * fscale);	// Get the values at a bigger scale
		int x = cvRound(i * xscale);
		Point ptNew = Point(b + x, h - (b + y));

		line(imageGraph, ptPrev, ptNew, graphColor, 1);	// Draw a line from the previous point to the new point
		ptPrev = ptNew;

		if(showPoints_)
		{
			char text[16];
			sprintf_s(text, sizeof(text) - 1, "(%d, %d)", ptNew.x, ptNew.y);

			putText(imageGraph, text, ptNew, CV_FONT_HERSHEY_COMPLEX, 0.3, BLACK, 1);
		}
	}

	// Write the graph label, if desired
	if(graphLabel != NULL && strlen(graphLabel) > 0) 
		putText(imageGraph, graphLabel, cvPoint(30, 20 * curvesNo), CV_FONT_HERSHEY_COMPLEX, 0.3, graphColor, 1);
}

void GraphUtils::DrawIntGraph(vector<int>& src, Mat& imageGraph, Scalar graphColor, const char* graphLabel, float minV, float maxV)
{
	vector<double> chainCodeGraph(src.begin(), src.end());

	DrawFloatGraph(chainCodeGraph, imageGraph, graphColor, graphLabel, minV, maxV);
}
