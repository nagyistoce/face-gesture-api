#include "ExpressionClassifier.h"

ExpressionClassifier::ExpressionClassifier(void)
{
}

ExpressionClassifier::ExpressionClassifier( int pOptions )
{
	if( pOptions & BLINKING_SKIN_COLOR )
	{
		CvSize eyeResSC = cvSize(150, 40);

		for(int i = 0; i < 5; i++)
		{
			_prevLeftEyesSC[i] = cvCreateImage(eyeResSC, IPL_DEPTH_8U, 1);
			_prevRightEyesSC[i] = cvCreateImage(eyeResSC, IPL_DEPTH_8U, 1);
		}
	}

	if( pOptions & BLINKING_OPTICAL_FLOW )
	{
		CvSize eyeResOF = cvSize(150, 40);

		_leOptFlow["gray"] =		cvCreateMat(eyeResOF.height, eyeResOF.width, CV_8UC1);
		_leOptFlow["prevgray"] =	cvCreateMat(eyeResOF.height, eyeResOF.width, CV_8UC1);
		_leOptFlow["flow"] =		cvCreateMat(eyeResOF.height, eyeResOF.width, CV_32FC2);
	}

	_mouthStatusThreshold = 0.4;

	_gaze =			cvPoint(-1,-1);
	_gazePrev =		cvPoint(-1,-1);
	_gazeCenter =	cvPoint(-1,-1);

	_sensitivity =	cvPoint(200, 100);

	_w1 = 0.5;
	_w2 = 0.5;

	_vMoving = 0;
	_hMoving = 0;

	_leftEyeBlinkRateSC = -1;
}

ExpressionClassifier::~ExpressionClassifier(void)
{
	map<string, CvMat*>::iterator p;

	for(int i = 0; i < 5; i++)
	{
		if(_prevLeftEyesSC[i] != NULL)
			cvReleaseImage( &_prevLeftEyesSC[i] );

		if(_prevRightEyesSC[i] != NULL)
			cvReleaseImage( &_prevRightEyesSC[i] );
	}

	for(p = _leOptFlow.begin(); p != _leOptFlow.end(); p++)
		cvReleaseMat( &p->second );
}

void ExpressionClassifier::MouthClassifierAAM( vector<CvPoint2D32f>& pImagePoints )
{
	CvPoint2D32f vB = cvPoint2D32f( ( pImagePoints[5].x + pImagePoints[6].x ) / 2.0,
									( pImagePoints[5].y + pImagePoints[6].y ) / 2.0 );

	double dstH = sqrt( pow( pImagePoints[0].x - pImagePoints[4].x, 2 ) + pow( pImagePoints[0].y - pImagePoints[4].y, 2 ) );
	double dstV = sqrt( pow( vB.x - pImagePoints[2].x, 2 ) + pow( vB.y - pImagePoints[2].y, 2 ) );

	_mouthAxisRate = dstV / dstH;
}

bool ExpressionClassifier::IsMouthOpen( void )
{
	return _mouthAxisRate > _mouthStatusThreshold;
}

void ExpressionClassifier::BlinkingClassifierSC( IplImage* pFrame, CvRect* pEyeRect, int pFrameNo, int pType )
{
	const double	vShrink	= 0.35;
	IplImage*		eyeRGB  = cvCreateImage(cvGetSize(_prevLeftEyesSC[0]), pFrame->depth, pFrame->nChannels);
	IplImage*		diff	= cvCreateImage(cvGetSize(_prevLeftEyesSC[0]), pFrame->depth, 1);
	CvRect			r;

	r.x			= pEyeRect->x;
	r.y			= cvRound( pEyeRect->y + pEyeRect->height * vShrink ) - pEyeRect->height / 5;
	r.width		= pEyeRect->width;
	r.height	= cvRound( pEyeRect->height - pEyeRect->height * 2 * vShrink );

	cvSetImageROI(pFrame, r);
	cvResize(pFrame, eyeRGB);
	cvResetImageROI(pFrame);

	cvSmooth(eyeRGB, eyeRGB, CV_GAUSSIAN);

	FaceTrackerUtil::ColorStrEqualize(eyeRGB, eyeRGB);

	cvCvtColor( eyeRGB, pType == LEFT_EYE ? _prevLeftEyesSC[pFrameNo%5] : _prevRightEyesSC[pFrameNo%5], CV_BGR2GRAY );

	if(pFrameNo > 5)
	{
		CvScalar mean, sdv;

		if( pType == LEFT_EYE )
		{
			cvCopy( _prevLeftEyesSC[0], diff );

			for(int i = 1; i < 5; i++)
				cvAnd(diff, _prevLeftEyesSC[i], diff);

			cvAvgSdv(diff, &mean, &sdv);
			_leftEyeBlinkRateSC = ( mean.val[0] + mean.val[1] + mean.val[2] ) / 3.0;
		}
		else if( pType == RIGHT_EYE )
		{
			cvCopy( _prevRightEyesSC[0], diff );

			for(int i = 1; i < 5; i++)
				cvAnd(diff, _prevRightEyesSC[i], diff);

			cvAvgSdv(diff, &mean, &sdv);
			_rightEyeBlinkRateSC = ( mean.val[0] + mean.val[1] + mean.val[2] ) / 3.0;
		}
	}
	
	cvReleaseImage(&eyeRGB);
	cvReleaseImage(&diff);
}

void ExpressionClassifier::BlinkingClassifierOF( IplImage* pFrame, CvRect* pEyeRect, int pFrameNo, int pType )
{
	const double	vShrink	=	0.2;
	IplImage*		resFrame =	cvCreateImage(cvSize(_leOptFlow["gray"]->cols, _leOptFlow["gray"]->rows), pFrame->depth, pFrame->nChannels);
	CvRect			r;
	CvMat*			temp; 

	r.x			= pEyeRect->x;
	r.y			= cvRound( pEyeRect->y + pEyeRect->height * vShrink );
	r.width		= pEyeRect->width;
	r.height	= cvRound( pEyeRect->height - pEyeRect->height * 2 * vShrink );

	cvSetImageROI(pFrame, r);
	cvResize(pFrame, resFrame);
	cvResetImageROI(pFrame);

	cvCvtColor(resFrame, _leOptFlow["gray"], CV_BGR2GRAY);

	if( pFrameNo > 0 )
	{
		cvCalcOpticalFlowFarneback(_leOptFlow["prevgray"], _leOptFlow["gray"], _leOptFlow["flow"], 0.5, 3, 3, 5, 7, 1.5, 0);
		drawOptFlowMap(_leOptFlow["flow"], 2);
	}
  
	CV_SWAP(_leOptFlow["prevgray"], _leOptFlow["gray"], temp);
	cvReleaseImage(&resFrame);
}

void ExpressionClassifier::drawOptFlowMap( const CvMat* pFlow, int pStep )
{
	IplImage* mask = cvCreateImage(cvSize(pFlow->cols, pFlow->rows), 8, 1);

	cvSetZero(mask);

	for( int y = 0; y < pFlow->rows; y += pStep)
		for( int x = 0; x < pFlow->cols; x += pStep)
		{
			CvPoint2D32f fxy = CV_MAT_ELEM(*pFlow, CvPoint2D32f, y, x);

			cvLine(mask, cvPoint(x,y), cvPoint(cvRound(x+fxy.x), cvRound(y+fxy.y)), cvScalarAll(255));
		}


	// TODO
	cout << cvAvg(mask).val[0] << endl;
	FaceTrackerUtil::ShowImage(mask, "eye mask", 1);

	cvReleaseImage(&mask);
}

CvScalar ExpressionClassifier::DetermineSkinColorFromEye(IplImage* pImg)
{
	IplImage*	skinReg  = NULL;
	CvRect		r;
	CvScalar	leftAvg, rightAvg;

	/* Extract skin region on the left */
	r.x			= 0;
	r.y			= 0;
	r.width		= cvRound(pImg->width*0.15);
	r.height	= pImg->height;

	skinReg = cvCreateImage(cvSize(r.width, r.height), pImg->depth, pImg->nChannels);
	cvSetImageROI(pImg, r);
	cvResize(pImg, skinReg);
	cvResetImageROI(pImg);
	/* Extract skin region on the left */

	leftAvg = cvAvg(skinReg);
	cvReleaseImage(&skinReg);

	/* Extract skin region on the left */
	r.x			= cvRound(pImg->width*0.85);
	r.y			= 0;
	r.width		= cvRound(pImg->width*0.15);
	r.height	= pImg->height;

	skinReg = cvCreateImage(cvSize(r.width, r.height), pImg->depth, pImg->nChannels);
	cvSetImageROI(pImg, r);
	cvResize(pImg, skinReg);
	cvResetImageROI(pImg);
	/* Extract skin region on the left */

	rightAvg = cvAvg(skinReg);
	cvReleaseImage(&skinReg);

	return cvScalar( (leftAvg.val[0] + rightAvg.val[0]) / 2.0, (leftAvg.val[1] + rightAvg.val[1]) / 2.0, (leftAvg.val[2] + rightAvg.val[2]) / 2.0 );
}

void ExpressionClassifier::GazeDirection( IplImage *pFrame, CvRect* pLER, CvRect* pRER, CvRect* pNR, CvRect* pMR )
{
	_gaze.x = _featureCenter.x = cvRound( ( ( pLER->x + pLER->width / 2.0 ) + 
		( pRER->x + pRER->width / 2.0 ) + 
		( pNR->x + pNR->width / 2.0 ) + 
		( pMR->x + pMR->width / 2.0 ) ) / 4.0 );

	_gaze.y = _featureCenter.y = cvRound( ( ( pLER->y + pLER->height / 2.0 ) + 
		( pRER->y + pRER->height / 2.0 ) + 
		( pNR->y + pNR->height / 2.0 ) + 
		( pMR->y + pMR->height / 2.0 ) ) / 4.0 );

	if( _gazeCenter.x == -1 && _gazeCenter.y == -1 )
		_gazeCenter = _gaze;

	_gaze.x -= _gazeCenter.x;
	_gaze.y -= _gazeCenter.y;

	_gaze.x *= pFrame->width / _sensitivity.x;
	_gaze.y *= pFrame->height / _sensitivity.y;

	_gaze.x += _gazeCenter.x;
	_gaze.y += _gazeCenter.y;

	/* smoothing */
	if( _gazePrev.x != -1 && _gazePrev.y != -1 )
	{
		_gaze.x = cvRound( _w1 * _gazePrev.x + _w2 * _gaze.x );
		_gaze.y = cvRound( _w1 * _gazePrev.y + _w2 * _gaze.y );

		_hMoving = abs( _gaze.x - _gazePrev.x ) > _sensitivity.x / 4 ? _gaze.x - _gazePrev.x : 0;
		_vMoving = abs( _gaze.y - _gazePrev.y ) > _sensitivity.y / 4 ? _gaze.y - _gazePrev.y : 0;
	}

	_gazePrev = _gaze;
}

void ExpressionClassifier::GazeDirection( IplImage *pFrame, vector<CvPoint2D32f> pFaceAamPoints )
{
    _gaze = cvPoint(0, 0);
    _featureCenter = cvPoint(0, 0);

    for(int i = 0; i < (int)pFaceAamPoints.size(); i++)
    {
        _gaze.x += cvRound( pFaceAamPoints[i].x );
        _featureCenter.x += cvRound( pFaceAamPoints[i].x );
        _gaze.y += cvRound( pFaceAamPoints[i].y );
        _featureCenter.y += cvRound( pFaceAamPoints[i].y );
    }

    _gaze.x = _gaze.x / pFaceAamPoints.size();
    _featureCenter.x = _featureCenter.x / pFaceAamPoints.size();
    _gaze.y = _gaze.y / pFaceAamPoints.size();
    _featureCenter.y = _featureCenter.y / pFaceAamPoints.size();

    if( _gazeCenter.x == -1 && _gazeCenter.y == -1 )
        _gazeCenter = _gaze;

    _gaze.x -= _gazeCenter.x;
    _gaze.y -= _gazeCenter.y;

    _gaze.x *= pFrame->width / _sensitivity.x;
    _gaze.y *= pFrame->height / _sensitivity.y;

    _gaze.x += _gazeCenter.x;
    _gaze.y += _gazeCenter.y;

    /* smoothing */
    if( _gazePrev.x != -1 && _gazePrev.y != -1 )
    {
        _gaze.x = cvRound( _w1 * _gazePrev.x + _w2 * _gaze.x );
        _gaze.y = cvRound( _w1 * _gazePrev.y + _w2 * _gaze.y );

        _hMoving = abs( _gaze.x - _gazePrev.x ) > _sensitivity.x / 4 ? _gaze.x - _gazePrev.x : 0;
        _vMoving = abs( _gaze.y - _gazePrev.y ) > _sensitivity.y / 4 ? _gaze.y - _gazePrev.y : 0;
    }

    _gazePrev = _gaze;
}

double	ExpressionClassifier::MouthAxisRate			( void ) const { return _mouthAxisRate; }
double	ExpressionClassifier::LeftEyeBlinkRateSC	( void ) const { return _leftEyeBlinkRateSC; }
double	ExpressionClassifier::RightEyeBlinkRateSC	( void ) const { return _rightEyeBlinkRateSC; }
double	ExpressionClassifier::MouthStatusThreshold	( void ) const { return _mouthStatusThreshold; }
CvPoint ExpressionClassifier::Gaze					( void ) const { return _gaze; }
CvPoint ExpressionClassifier::FeatureCenter			( void ) const { return _featureCenter; }
int		ExpressionClassifier::VMoving				( void ) const { return _vMoving; }
int		ExpressionClassifier::HMoving				( void ) const { return _hMoving; }

void	ExpressionClassifier::MouthStatusThreshold	( double pVal ) { _mouthStatusThreshold = pVal; }
