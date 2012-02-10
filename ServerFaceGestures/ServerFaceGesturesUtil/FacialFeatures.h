#pragma once

#ifndef _FACIALFEATURES_H_
#define _FACIALFEATURES_H_

#include "FaceTrackerUtil.h"

class HaarDetector;

class FACE_API FacialFeatures
{
public:
	FacialFeatures		( void );
	FacialFeatures		( int pOptions, string pResourcePath );
	~FacialFeatures		( void );

	void DetectFace		( IplImage* pFrame, int pTracking );
	void DivideFace		( IplImage* pFrame );
	void DetectFeatures	( IplImage* pFrame, int pTracking );
    void CreateStat		( IplImage* pFrame, CvScalar pRGB );
	void SaveFeatures	( IplImage* pFrame );

	CvRect* FaceRect	( void ) const;
	CvRect* LeftEyeRect	( void ) const;
	CvRect* RightEyeRect( void ) const;
	CvRect* NoseRect	( void ) const;
	CvRect* MouthRect	( void ) const;

private:
	IplImage*		_faceParts[4]; // Upper-Left, Upper-Right, Lower, Middle face region;

	int				_featureCounter;

	HaarDetector*	_faceDetector;
	HaarDetector*	_rightEyeDetector;
	HaarDetector*	_leftEyeDetector;
	HaarDetector*	_mouthDetector;
	HaarDetector*	_noseDetector;

	CvRect*			_faceRect;
	CvRect*			_leftEyeRect;
	CvRect*			_rightEyeRect;
	CvRect*			_noseRect;
	CvRect*			_mouthRect;

	bool			_detectFace;
	bool			_detectLeftEye;
	bool			_detectRightEye;
	bool			_detectNose;
	bool			_detectMouth;
};

#endif //_FACIALFEATURES_H_
