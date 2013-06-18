#pragma once

/*!
	\defgroup AAMInterface Interface to the Active Appearance Models library.
*/

#include "AAM_Shape.h"
#include "AAM_IC.h"
#include "AAM_basic.h"


namespace dolslr { 


class HaarDetector;

typedef std::vector<cv::Point2d> Point2dVec;

//! Class for handling AAMLibrary.
/*!
	\ingroup AAMInterface
*/
extern "C++" class __declspec(dllexport) AAMInterface
{
public:
	//! Enumerations for choosing drawing mode.
	enum DrawingOptions 
	{ 
		DRAW_POINT = 0,			//<! Draw just the shape points
		DRAW_TRIANGLE = 1,		//<! Draw the mesh
		DRAW_APPEARANCE = 2		//<! Draw the full appearance
	};

	//! Constructor
	/*
		\param type Type of the current AAM interface
		\param fileNode Configuration for the detection and fitting processes
	*/
	AAMInterface(const std::string& type, const cv::FileNode& fileNode);

	//! Destructor
	~AAMInterface(void);

	//! Fitting the model over the current frame
	/*
		\param frame Current frame
		\param rect ROI of the Haar detector
		\param drawStyle AAM drawing style
	*/
	void Process(cv::Mat& frame, cv::Rect& rect = cv::Rect(), int drawStyle = DRAW_POINT);

	//! Smoothing the result of Haar detector (average filter)
	void SmoothBoundingBox(void);

	//! Gets the bounding box of the feature
	cv::Rect BoundingBox(void) const; 

	AAM*		combinedModel;	//!< The combined AAM model.
	Point2dVec	imagePoints;	//!< The shape points.

protected:
	//! Make the ones face box smaller and move it down a bit
	/*
		\param Shape Shape for adjusting
	*/
	void AdjustShape(AAM_Shape& Shape);

	//! Determine the bounding box of the feature
	/*
		\param frame Current frame
		\param rect ROI of the Haar detector
	*/
	void Detect(cv::Mat& frame, cv::Rect& rect);

private:
	//! Fitting the model over the current frame
	/*
		\param frame Current frame
		\param drawStyle AAM drawing style
	*/
	void Fit(cv::Mat& frame, int drawStyle = DRAW_POINT);

	//! Loading the AAM model file (PATH/DolSLR/Testing/Futtatas/XML/DeformableModels/something.aam_basic).
	void LoadModel();

	//! Detect an object on the image
	/*
		\param pMeanShape Mean shape of the feature
		\param rect Bounding box of the feature
	*/
	AAM_Shape InitShape(const AAM_Shape& pMeanShape, cv::Rect& rect);

	//! Align MeanShape to the bounding box
	/*
		\param StartShape Start-up shape
		\param DetShape Determined shape
		\param MeanShape Mean shape
	*/
	void AlignShape(AAM_Shape& StartShape, const AAM_Shape& DetShape, const AAM_Shape& MeanShape);

	HaarDetector* detector_;		//!< General objectdetector (Viola-Jones).
	std::string type_;				//!< Type name of the algorithm.
	std::string aamModelFileName_;	//!< Path of the AAM model file
	cv::Point2d shift_;				//!< Shifting variable for initialize the process
	cv::Point2d shrink_;			//!< Shrinking variable for initialize the process
	cv::Rect boundingBox_;			//!< Bounding box of the feature

	AAM_Shape currentShape;			//!< Current AAM shape

	cv::Rect prevBoundingBox_;		//!< Bounding box over the previous frame
	cv::Point2d w_;					//!< Weight for smoothing Haar detection
};

}
