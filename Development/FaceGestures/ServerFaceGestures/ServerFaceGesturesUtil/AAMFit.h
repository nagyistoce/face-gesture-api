#pragma once

#ifndef _AAMFIT_H_
#define _AAMFIT_H_

#include "AAM_Shape.h"
#include "AAM_IC.h"
#include "AAM_basic.h"

#define AAM_FIT_FACE		32
#define AAM_FIT_MOUTH		64

using namespace std;

class FACE_API AAMFit
{
public:
	AAMFit	( void );
	~AAMFit	( void );

	void					LoadModel			( string pModelFileName );						// Loading the AAM model file (AAM.Models\something.amf).
	void					Fit					( IplImage* pFrame, CvRect *pR, int pType );	// Fitting the model over the current frame.
	AAM*					Model				( void ) const;
	vector<CvPoint2D32f>	ImagePoints			( void ) const;
	void					CreateStat			( IplImage *pFrame, int pMode, bool pDrawNumbers, CvScalar pRGB );
	void					SmoothImagePoints	( void );

private:
	AAM*					_model;					// The combined AAM model.
	vector<CvPoint2D32f>	_imagePoints;			// The shape points.
	vector<CvPoint2D32f>	_prevImagePoints;		// The previous shape points
	vector<CvPoint2D32f>	_smoothedImagePoints;	// The smoothed shape points
	double					_w1;
	double					_w2;

	AAM_Shape	InitShape		( const AAM_Shape& pMeanShape, CvRect* pR, int pType );								// Detect an object on the image.
	void		AdjustFaceShape	( AAM_Shape& Shape );																// Make the ones face box smaller and move it down a bit.
	void		AdjustMouthShape( AAM_Shape& pShape );																// Make the ones mouth box smaller.
	void		AlignShape		( AAM_Shape& StartShape, const AAM_Shape& DetShape, const AAM_Shape& MeanShape );	// Align MeanShape to the bounding box.
};

#endif //_AAMFIT_H_
