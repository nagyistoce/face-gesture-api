#include "FacialFeatures.h"

FacialFeatures::FacialFeatures( void )
{
}

FacialFeatures::FacialFeatures( int pOptions, string pResourcePath )
{
	_detectFace =		pOptions & DETECT_FACE ? true : false;
	_detectLeftEye =	pOptions & DETECT_LEFT_EYE ? true : false;
	_detectRightEye =	pOptions & DETECT_RIGHT_EYE ? true : false;
	_detectNose =		pOptions & DETECT_NOSE ? true : false;
	_detectMouth =		pOptions & DETECT_MOUTH ? true : false;


	if( _detectFace )
	{
		_faceDetector =		new HaarDetector(pResourcePath + OPENCV_CASCADES + "/haarcascade_frontalface_default.xml");
		_faceDetector->SetMinsize( cvSize( 150, 150 ) );
	}

	if( _detectLeftEye )
	{
		_leftEyeDetector =	new HaarDetector(pResourcePath + OPENCV_CASCADES + "/haarcascade_mcs_lefteye.xml");
		_leftEyeDetector->SetMinsize( cvSize(72,48) );
	}

	if( _detectRightEye )
	{
		_rightEyeDetector = new HaarDetector(pResourcePath + OPENCV_CASCADES + "/haarcascade_mcs_righteye.xml");
		_rightEyeDetector->SetMinsize( cvSize(72,48) );
	}

	if( _detectNose )
	{
		_noseDetector  =	new HaarDetector(pResourcePath + OPENCV_CASCADES + "/haarcascade_mcs_nose.xml");
		_noseDetector->SetMinsize( cvSize(72,60) );
	}

	if( _detectMouth )
	{
		_mouthDetector =	new HaarDetector(pResourcePath + OPENCV_CASCADES + "/haarcascade_mcs_mouth.xml");
		_mouthDetector->SetMinsize( cvSize(72,60) );
	}

	_faceRect = _leftEyeRect = _rightEyeRect = _noseRect = _mouthRect = NULL;

	for( int i = 0; i < 4; i++ )
		_faceParts[i] = NULL;

	_featureCounter = -1;
}

FacialFeatures::~FacialFeatures( void )
{
	if(_detectFace)		delete _faceDetector;
	if(_detectRightEye)	delete _rightEyeDetector;
	if(_detectLeftEye)	delete _leftEyeDetector;
	if(_detectMouth)	delete _mouthDetector;
	if(_detectNose)		delete _noseDetector;

	for( int i = 0; i < 4; i++ )
		if( _faceParts[i] )
			cvReleaseImage( &_faceParts[i] );
}

void FacialFeatures::DetectFace( IplImage * pFrame, int pTracking )
{
	if(!_detectFace || !( pTracking & DETECT_FACE ) )
		return;

	CvSeq* seqFace = _faceDetector->Detect( pFrame );

	// Get the first face
	if( seqFace && seqFace->total > 0 ) 
	{
		_faceRect = (CvRect*)cvGetSeqElem( seqFace, 0 );
	}
	else
	{
		_faceRect = NULL;
	}
}


// faceUpperLeft, faceUpperRight, faceLower, faceMiddle;
void FacialFeatures::DivideFace( IplImage* pFrame )
{
	IplImage* face;

	face = cvCreateImage( cvSize(_faceRect->width, _faceRect->height), pFrame->depth, pFrame->nChannels );
	cvGetRectSubPix(pFrame, face, cvPoint2D32f(_faceRect->x + _faceRect->width * 0.5, _faceRect->y + _faceRect->height * 0.5));

	_faceParts[0]  = cvCreateImage(cvSize( static_cast<int>( _faceRect->width * 0.6 ), 
		static_cast<int>( _faceRect->height * 0.6 ) ), pFrame->depth, pFrame->nChannels );
	_faceParts[1] = cvCreateImage(cvSize( static_cast<int>( _faceRect->width * 0.6 ), 
		static_cast<int>( _faceRect->height * 0.6 ) ), pFrame->depth, pFrame->nChannels );

	_faceParts[2] = cvCreateImage( cvSize( _faceRect->width, _faceRect->height / 3 ), pFrame->depth, pFrame->nChannels );
	_faceParts[3] = cvCreateImage( cvSize( _faceRect->width, _faceRect->height / 2 ), pFrame->depth, pFrame->nChannels );

	cvGetRectSubPix(pFrame, _faceParts[0], cvPoint2D32f(_faceRect->x + _faceRect->width * 0.3, _faceRect->y + _faceRect->height * 0.3));
	cvGetRectSubPix(pFrame, _faceParts[1], cvPoint2D32f(_faceRect->x + _faceRect->width * 0.7, _faceRect->y + _faceRect->height * 0.3));
	cvGetRectSubPix(pFrame, _faceParts[2], cvPoint2D32f(_faceRect->x + _faceRect->width * 0.5, _faceRect->y + _faceRect->height * 0.85));
	cvGetRectSubPix(pFrame, _faceParts[3], cvPoint2D32f(_faceRect->x + _faceRect->width * 0.5, _faceRect->y + _faceRect->height * 0.5));

	cvReleaseImage(&face);
}

// Upper-Left, Upper-Right, Lower, Middle face region;
void FacialFeatures::DetectFeatures( IplImage* pFrame, int pTracking )
{
	CvSeq* seq = NULL;

	DivideFace(pFrame);

	// Detecting the nose
	seq = _detectNose && (pTracking & DETECT_NOSE) ? _noseDetector->Detect( _faceParts[3] ) : NULL;

	if( seq && seq->total > 0 ) 
	{
		_noseRect = (CvRect*)cvGetSeqElem( seq, 0 );

		_noseRect->x += _faceRect->x;
		_noseRect->y += static_cast<int>( _faceRect->y + _faceRect->height * 0.25 );
	} 
	else 
	{
		_noseRect = NULL;
	}

	// Detecting the left eye
	seq = _detectLeftEye && (pTracking & DETECT_LEFT_EYE) ? _leftEyeDetector->Detect( _faceParts[0] ) : NULL;

	if( seq && seq->total > 0 )
	{
		_leftEyeRect = (CvRect*)cvGetSeqElem( seq, 0 );

		_leftEyeRect->x += _faceRect->x;
		_leftEyeRect->y += _faceRect->y;
	}
	else 
	{
		_leftEyeRect = NULL;
	}

	// Detecting the right eye
	seq = _detectRightEye && (pTracking & DETECT_RIGHT_EYE) ? _rightEyeDetector->Detect( _faceParts[1] ) : NULL;

	if( seq && seq->total > 0 )
	{
		_rightEyeRect = (CvRect*)cvGetSeqElem(seq, 0);

		_rightEyeRect->x += static_cast<int>( _faceRect->x + _faceRect->width * 0.4 );
		_rightEyeRect->y += _faceRect->y;
	}
	else 
	{
		_rightEyeRect = NULL;
	}

	// Detecting the mouth
	seq = _detectMouth && (pTracking & DETECT_MOUTH) ? _mouthDetector->Detect( _faceParts[2] ) : NULL;

	if( seq && seq->total > 0 )
	{
		_mouthRect = (CvRect*)cvGetSeqElem(seq, 0);

		_mouthRect->x += _faceRect->x; 
		_mouthRect->y += static_cast<int>( _faceRect->y + _faceRect->height * 0.65 );
	}
	else 
	{
		_mouthRect = NULL;
	}

	for( int i = 0; i < 4; i++ )
		cvReleaseImage( &_faceParts[i] );
}

void FacialFeatures::SaveFeatures( IplImage* pFrame )
{
	stringstream ss;
	string lz = "";
	int p[3];

	p[0] = CV_IMWRITE_JPEG_QUALITY;
	p[1] = 100;
	p[2] = 0;

	_featureCounter++;

	if( _featureCounter < 10 )
		lz = "000";
	else if(_featureCounter < 100)
		lz = "00";
	else if(_featureCounter < 1000)
		lz = "0";

	if( _faceRect )
	{
		cvSetImageROI(pFrame, *_faceRect);
		ss.str("");
		ss << "result/face/face." << lz << _featureCounter << ".jpg";
		cvSaveImage(ss.str().c_str(), pFrame, p);
	}

	if(_faceRect && _noseRect)
	{
		cvSetImageROI(pFrame, *_noseRect);
		ss.str("");
		ss << "result/nose/nose." << lz << _featureCounter << ".jpg";
		cvSaveImage(ss.str().c_str(), pFrame, p);
	}

	if(_faceRect && _leftEyeRect)
	{
		cvSetImageROI(pFrame, *_leftEyeRect);
		ss.str("");
		ss << "result/letf.eye/letf.eye." << lz << _featureCounter << ".jpg";
		cvSaveImage(ss.str().c_str(), pFrame, p);
	}

	if(_faceRect && _rightEyeRect)
	{
		cvSetImageROI(pFrame, *_rightEyeRect);
		ss.str("");
		ss << "result/right.eye/right.eye." << lz << _featureCounter << ".jpg";
		cvSaveImage(ss.str().c_str(), pFrame, p);
	}

	if(_faceRect && _mouthRect)
	{
		cvSetImageROI(pFrame, *_mouthRect);
		ss.str("");
		ss << "result/mouth/mouth." << lz << _featureCounter << ".jpg";
		cvSaveImage(ss.str().c_str(), pFrame, p);
	}

	cvResetImageROI(pFrame);
}


void FacialFeatures::CreateStat( IplImage* pFrame, CvScalar pRGB )
{	
	if( _faceRect )
    {
		cvRectangle( pFrame, cvPoint( _faceRect->x, _faceRect->y ), cvPoint( _faceRect->x + _faceRect->width, _faceRect->y + _faceRect->height ), pRGB );

	    if( _noseRect )
		    cvRectangle( pFrame, cvPoint( _noseRect->x, _noseRect->y ), cvPoint( _noseRect->x + _noseRect->width, _noseRect->y + _noseRect->height ), pRGB );

	    if( _leftEyeRect )
		    cvRectangle( pFrame, cvPoint( _leftEyeRect->x, _leftEyeRect->y ), cvPoint( _leftEyeRect->x + _leftEyeRect->width, _leftEyeRect->y + _leftEyeRect->height ), pRGB );

	    if( _rightEyeRect )
		    cvRectangle( pFrame, cvPoint( _rightEyeRect->x, _rightEyeRect->y ), cvPoint( _rightEyeRect->x + _rightEyeRect->width, _rightEyeRect->y + _rightEyeRect->height ), pRGB );

	    if( _mouthRect )
		    cvRectangle( pFrame, cvPoint( _mouthRect->x, _mouthRect->y ), cvPoint( _mouthRect->x + _mouthRect->width, _mouthRect->y + _mouthRect->height ), pRGB );
    }
}

CvRect* FacialFeatures::FaceRect( void ) const 
{ 
	return _faceRect; 
}

CvRect* FacialFeatures::LeftEyeRect( void ) const 
{ 
	return _leftEyeRect; 
}

CvRect* FacialFeatures::RightEyeRect( void ) const 
{ 
	return _rightEyeRect; 
}

CvRect* FacialFeatures::NoseRect( void ) const 
{ 
	return _noseRect; 
}

CvRect* FacialFeatures::MouthRect( void ) const 
{ 
	return _mouthRect; 
}
