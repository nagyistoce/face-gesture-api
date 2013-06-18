#pragma once

#ifndef _IFACETRACKER_H_
#define _IFACETRACKER_H_

#include "FacialFeatures.h"
#include "AAMFit.h"
#include "ExpressionClassifier.h"
#include "HeadPose.h"

class FACE_API IFaceTracker
{
public:
	IFaceTracker		( void );
	IFaceTracker		( int pOptions, string pFaceAAM, string pMouthAAM, string pResourcePath );
	~IFaceTracker		( void );

	void		TrackFeatures	( IplImage *pFrame, int pTracking, int pExpression );
	void		CreateStat		( IplImage *pFrame, int pTracking, int pExpression );
	IplImage*	OutputImage		( void ) const;
    string      Message         ( void ) const;

private:
    void        CreateMessage   ( int pTracking, int pExpression );

	FacialFeatures*			_ff;

    HeadPose*               _headPose;

	AAMFit*					_faceAAM;
	AAMFit*					_mouthAAM;

	int						_iFaceOpt;
	int						_expClassOpt;
	ExpressionClassifier*	_expClass;

	IplImage*				_outputImage;
	CvVideoWriter*			_videoWriter;

	CvFont					_font;
    string                  _message;

	int						_frameCounter;
};

#endif //_IFACETRACKER_H_
