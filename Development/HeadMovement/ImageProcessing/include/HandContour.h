#pragma once
#pragma warning( disable: 4251 ) // Disable needs to have dll-interface...

#include "opencv2/core/core.hpp"

class Contour;
class GraphUtils;
class MatWindow;
class WindowGroupButton;

//! Class for analyzing the hand contours.
/*!
	\ingroup Core

	Finds the biggest contour over the current frame, approximates it with a polygon.
	Finally we get the convex hull and the bounding rectangle, of the biggest contour.

	This class is also for draw and compute the number of fingers. It is based some simple rule.
*/
extern "C++" class __declspec(dllexport) HandContour
{
public:
	//! Constructor
	HandContour(WindowGroupButton* button);

	//! Constructor
	/*!
		\param name Name of the hand.
	*/
	HandContour(WindowGroupButton* button, const std::string& name);

	//! Destructor
	~HandContour(void);

	//! Finds convex hull of the fitted active contour over the biggest contour and takes some processing with these.
	/*!
		\param wholeFrameRoi The hand ROI image. This is also for drawing the active contour fitting results.
		\param handMask The gray scale hand mask.
	*/
	void ActiveContourFitting(cv::Mat& wholeFrameRoi, cv::Mat& handMask);

	//! Finds the intersection of elbow-hand center line and hand contour. Then it rotates this point to chainCode[0].
	/*!
		\param fullFrame The whole input frame.
		\param handRoi The hand ROI.
		\param elbowRoi The elbow ROI.
	*/
	void ActiveContourStandardizing(cv::Mat& fullFrame, cv::Rect& handRoi, cv::Rect& elbowRoi);

	//! Shows the three chain graphs on a new image.
	/*!
		\param showChain Shows the discrete chain graph.
		\param showSmoothed Shows the smoothed chain graph.
		\param showFirstDiff Shows the first derivate of the chain graph.
		\param showSecondDiff Shows the second derivate of the chain graph.
	*/
	void ShowChainGraphs(bool showChain, bool showSmoothed, bool showFirstDiff, bool showSecondDiff);

	//! Mirrors the three chain graphs over the x axis.
	void MirrorChains(void);

	//! Getting, smoothing and deriving the chain code (from the active contour).
	void GetAndSmoothAndDeriveChain(void);

	std::vector<int>						hull;					//<! The convex hull. It is either an integer vector of indices or vector of points.
	std::vector<cv::Point>					activeContour;			//<! The active contour. Vector of 2D points.

	std::vector<int>						chainCode;				//<! Container for the chain code representation.
	std::vector<double>						chainCodeSmoothed;		//<! The smoothed (with a moving average) chain code.	
	std::vector<double>						chainCodeFirstDiff;		//<! The first derivate of chain values.
	std::vector<double>						chainCodeSecondDiff;	//<! The second derivate of chain values.

	cv::RotatedRect							minRect;				//<! Rotated rectangle of the minimum area enclosing the input 2D point set (contour).
	int										fingerNum;				//<! The number of fingers.

private:
	//! Method for drawing and computing the number of fingers. It is based some simple rule.
	/*!
		\param wholeFrameRoi ROI of the hand contour over the current frame.
	*/
	void DrawAndComputeFingersNum(cv::Mat& wholeFrameRoi);

	//! Visualize the results of contour analyzing.
	/*!
		\param wholeFrameRoi ROI of the hand contour over the current frame.
		\param handContour The largest hand contour.
		\param hull The convex hull of the hand contour.
	*/
	void DrawContours(cv::Mat& wholeFrameRoi, std::vector<cv::Point>& handContour, std::vector<cv::Point>& hull);

	std::string								name_;				//<! Name of the hand.

	std::vector<float>						depth_;				//<! The farthest from the convex hull point within the defect.
	std::vector<cv::Point>					startPoint_;		//<! Point of the contour where the defect begins.
	std::vector<cv::Point>					endPoint_;			//<! Point of the contour where the defect ends.
	std::vector<cv::Point>					depthPoint_;		//<! Distance between the farthest point and the convex hull.

	cv::Mat									output_;			//<! The output frame for visualizing the results.
	Contour*								contour_;			//<! Object for doing some contour analysis.
	GraphUtils*								chainPlotter_;		//<! Chain graph plotter.

	MatWindow*								window_;
};
