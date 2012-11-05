#pragma once
#pragma warning( disable: 4251 ) // Disable needs to have dll-interface...
#pragma warning( disable: 4244 ) // Disable conversion from 'int' to 'const float', possible loss of data

#include "opencv2/core/core.hpp"

//! Class for drawing graphs.
/*!
	\ingroup Core

	Graphing functions for OpenCV.
*/
extern "C++" class __declspec(dllexport) GraphUtils
{
public:
	//! Constructor
	GraphUtils(void);

	//! Constructor
	/*!
		\param graphSize Resolution of the graph.
		\param showScale Whether it shows the scale or not.
		\param showPoints Whether it shows the points or not.
	*/
	GraphUtils(cv::Size graphSize, bool showScale, bool showPoints);

	//! Destructor
	~GraphUtils(void);

	//! Draw the graph of an array of floats into a new image.
	/*!
		\param src Points of the graph.
		\param imageGraph The graph image.
		\param graphColor Graph color.
		\param graphLabel Label of the current graph.
		\param minV Minimum value of the y axis.
		\param maxV Maximum value of the y axis.
	*/
	void DrawFloatGraph(std::vector<double>& src, cv::Mat& imageGraph, cv::Scalar graphColor, const char* graphLabel, float minV = FLT_MIN, float maxV = FLT_MAX);

	//! Draw the graph of an array of integers into a new image.
	/*!
		\param src Points of the graph.
		\param imageGraph The graph image.
		\param graphColor Graph color.
		\param graphLabel Label of the current graph.
		\param minV Minimum value of the y axis.
		\param maxV Maximum value of the y axis.
	*/
	void DrawIntGraph(std::vector<int>& src, cv::Mat& imageGraph, cv::Scalar graphColor, const char* graphLabel, float minV = FLT_MIN, float maxV = FLT_MAX);

private:
	cv::Size graphSize_;	//<! Size of the graph.
	bool showScale_;		//<! Draw the axes measures or not.
	bool showPoints_;		//<! Draw the coordinates or not.
	int curvesNo;			//<! Number of the curves over a graph image (for multiple data drawing).
};
