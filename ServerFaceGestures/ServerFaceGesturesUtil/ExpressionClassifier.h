#pragma once
#pragma warning( disable: 4251 )

#ifndef _EXPRESSIONCLASSIFIER_H_
#define _EXPRESSIONCLASSIFIER_H_

#include "FaceTrackerUtil.h"
#include <map>

#define BLINKING_SKIN_COLOR			1
#define BLINKING_OPTICAL_FLOW		2
#define ESTIMATE_GAZE_DIRECTION		4
#define MOUTH_OPEN_CLOSE			8
#define ESTIMATE_HEAD_POSE	        16

#define LEFT_EYE 0
#define RIGHT_EYE 1

class FACE_API ExpressionClassifier
{
public:
	ExpressionClassifier	( void );
	ExpressionClassifier	( int pOptions );
	~ExpressionClassifier	( void );

	void	MouthClassifierAAM		( vector<CvPoint2D32f>&	pImagePoints );
	bool	IsMouthOpen				( void );

	void	BlinkingClassifierSC	( IplImage* pFrame, CvRect* pEyeRect, int pFrameNo, int pType = LEFT_EYE );
	void	BlinkingClassifierOF	( IplImage* pFrame, CvRect* pEyeRect, int pFrameNo, int pType = LEFT_EYE );

	void	GazeDirection			( IplImage *pFrame, CvRect* pLER, CvRect* pRER, CvRect* pNR, CvRect* pMR );
    void    GazeDirection           ( IplImage *pFrame, vector<CvPoint2D32f> pFaceAamPoints );

	double	MouthAxisRate			( void ) const;
	double	LeftEyeBlinkRateSC		( void ) const;
	double	RightEyeBlinkRateSC		( void ) const;
	double	MouthStatusThreshold	( void ) const;
	CvPoint Gaze					( void ) const;
	CvPoint FeatureCenter			( void ) const;
	int		VMoving					( void ) const;
	int		HMoving					( void ) const;

	void	MouthStatusThreshold	( double pVal );

private:
	IplImage*	_prevLeftEyesSC[5];
	IplImage*	_prevRightEyesSC[5];

	map<string, CvMat*> _leOptFlow;

	double		_mouthAxisRate;
	double		_leftEyeBlinkRateSC;
	double		_rightEyeBlinkRateSC;
	double		_mouthStatusThreshold;

	double		_w1;
	double		_w2;

	CvPoint		_gaze;
	CvPoint		_gazePrev;
	CvPoint		_gazeCenter;
	CvPoint		_featureCenter;
	CvPoint		_sensitivity;

	int			_vMoving;
	int			_hMoving;

	CvScalar	DetermineSkinColorFromEye	( IplImage* pImg );
	void		drawOptFlowMap				( const CvMat* pFlow, int pStep );

};

#endif //_EXPRESSIONCLASSIFIER_H_
