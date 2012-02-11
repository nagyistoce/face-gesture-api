#include "IFaceTracker.h"
#include <sstream>

IFaceTracker::IFaceTracker( void )
{
}

IFaceTracker::IFaceTracker( int pOptions, string pFaceAAM, string pMouthAAM, string pResourcePath )
{
	_iFaceOpt =		pOptions;
	_expClassOpt =	0 | BLINKING_SKIN_COLOR | BLINKING_OPTICAL_FLOW | ESTIMATE_GAZE_DIRECTION | MOUTH_OPEN_CLOSE | ESTIMATE_HEAD_POSE;

	_ff =			new FacialFeatures( pOptions, pResourcePath );
	_expClass =		new ExpressionClassifier( _expClassOpt );

    if(( _expClassOpt & ESTIMATE_HEAD_POSE ))
        _headPose = new HeadPose();

	_outputImage =	NULL;
	_frameCounter = 0;
    _message = "";

	cvInitFont( &_font, CV_FONT_HERSHEY_SIMPLEX, 0.1, 1.0, 0, 1, CV_AA );

	if( _iFaceOpt & AAM_FIT_FACE )
	{
		_faceAAM = new AAMFit(); 
		_faceAAM->LoadModel( pResourcePath + AAM_MODELS + pFaceAAM );
	}

	if( _iFaceOpt & AAM_FIT_MOUTH )
	{
		_mouthAAM = new AAMFit();
		_mouthAAM->LoadModel( pResourcePath + AAM_MODELS + pMouthAAM );
	}

	// http://www.fourcc.org/codecs.php
	if( _iFaceOpt & SAVE_OUTPUT )
		_videoWriter = cvCreateVideoWriter( "results.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, cvSize( 1024, 768 ), 1 );
}

IFaceTracker::~IFaceTracker( void )
{
	delete _ff; 
    delete _headPose;
	delete _expClass;

	if( _iFaceOpt & AAM_FIT_FACE )
		delete _faceAAM;

	if( _iFaceOpt & AAM_FIT_MOUTH )
		delete _mouthAAM;

	if(_outputImage)
		cvReleaseImage( &_outputImage );

	if(_iFaceOpt & SAVE_OUTPUT)
		cvReleaseVideoWriter( &_videoWriter );
}

void IFaceTracker::TrackFeatures( IplImage *pFrame, int pTracking, int pExpression )
{
	// Face detection.
    if(( _iFaceOpt & DETECT_FACE ) && ( pTracking & DETECT_FACE ))
	    _ff->DetectFace( pFrame, pTracking );

	// Detects the 4 distinctive facial feature (eyes, nose, mouth) with Haar-like features.
	if(_ff->FaceRect())
	{
		_ff->DetectFeatures( pFrame, pTracking );

		// Estimate the gaze's direction (based on features).
		//if( ( _expClassOpt & ESTIMATE_GAZE_DIRECTION ) && ( pExpression & ESTIMATE_GAZE_DIRECTION ) && _ff->LeftEyeRect() && _ff->RightEyeRect() && _ff->NoseRect() && _ff->MouthRect())
		//	_expClass->GazeDirection( pFrame, _ff->LeftEyeRect(), _ff->RightEyeRect(), _ff->NoseRect(), _ff->MouthRect() );
	}

	// Fits the face with AAM
	if( ( _iFaceOpt & AAM_FIT_FACE ) && ( pTracking & AAM_FIT_FACE ) && _faceAAM && _ff->FaceRect() )
	{
		_faceAAM->Fit( pFrame, _ff->FaceRect(), AAM_FIT_FACE );
		_faceAAM->SmoothImagePoints();

        if(( _iFaceOpt & ESTIMATE_HEAD_POSE ) && ( pExpression & ESTIMATE_HEAD_POSE ))
        {
            _headPose->ImagePoints(_faceAAM->ImagePoints());
            _headPose->EstimateHeadPose();
        }

        if( ( _expClassOpt & ESTIMATE_GAZE_DIRECTION ) && ( pExpression & ESTIMATE_GAZE_DIRECTION ))
            _expClass->GazeDirection( pFrame, _faceAAM->ImagePoints() );
	}

	// Fits the mouth with AAM
	if( ( _iFaceOpt & AAM_FIT_MOUTH ) && ( pTracking & AAM_FIT_MOUTH ) && _mouthAAM && _ff->MouthRect() )
	{
		_mouthAAM->Fit( pFrame, _ff->MouthRect(), AAM_FIT_MOUTH );
		_mouthAAM->SmoothImagePoints();

		// Classify the mouth (whether it is open or closed)
		if( ( _expClassOpt & MOUTH_OPEN_CLOSE ) && ( pExpression & MOUTH_OPEN_CLOSE ) )
			_expClass->MouthClassifierAAM( _mouthAAM->ImagePoints() );
	}

	// Blink detection on the left eye
	if( _ff->LeftEyeRect() )
	{
		// Based on skin color
		if( ( _expClassOpt & BLINKING_SKIN_COLOR ) && ( pExpression & BLINKING_SKIN_COLOR ) )
			_expClass->BlinkingClassifierSC( pFrame, _ff->LeftEyeRect(), _frameCounter, LEFT_EYE );

		// Based on optical flow
		if( ( _expClassOpt & BLINKING_OPTICAL_FLOW ) && ( pExpression & BLINKING_OPTICAL_FLOW ) )
			_expClass->BlinkingClassifierOF( pFrame, _ff->LeftEyeRect(), _frameCounter, LEFT_EYE );
	}

	// Blink detection on the right eye
	if( _ff->RightEyeRect() )
	{
		// Based on skin color
		if( ( _expClassOpt & BLINKING_SKIN_COLOR ) && ( pExpression & BLINKING_SKIN_COLOR ) )
			_expClass->BlinkingClassifierSC( pFrame, _ff->RightEyeRect(), _frameCounter, RIGHT_EYE );
	}

	// Saving facial features
	if(_iFaceOpt & SAVE_FEATURES)
	{
		_ff->SaveFeatures( pFrame );
	}

    CreateMessage(pTracking, pExpression);
	CreateStat( pFrame, pTracking, pExpression );

	// Stream the output
	if( _iFaceOpt & SAVE_OUTPUT )
	{
		cvWriteFrame( _videoWriter, _outputImage );
	}

	_frameCounter++;
}

void IFaceTracker::CreateStat(IplImage *pFrame, int pTracking, int pExpression)
{
	if( _outputImage )
		cvReleaseImage( &_outputImage );

	_outputImage = cvCloneImage( pFrame );

	// Face AAM stat
	if( ( _iFaceOpt & AAM_FIT_FACE ) && ( pTracking & AAM_FIT_FACE ) && _ff->FaceRect() )
    {
		_faceAAM->CreateStat( _outputImage, 1, false, CV_RGB(0,0,255) );

        if(( _iFaceOpt & ESTIMATE_HEAD_POSE ) && ( pExpression & ESTIMATE_HEAD_POSE ))
            _headPose->CreateStat(_outputImage, _ff->FaceRect());

        // Gaze direction stat
        if(( _expClassOpt & ESTIMATE_GAZE_DIRECTION ) && ( pExpression & ESTIMATE_GAZE_DIRECTION ))
        {
            cvCircle(_outputImage, _expClass->Gaze(), 3, CV_RGB(0,0,255), -1);
            cvLine(_outputImage, _expClass->FeatureCenter(), _expClass->Gaze(), CV_RGB(0,0,255), 1, CV_AA);
            cvCircle(_outputImage, _expClass->FeatureCenter(), 3, CV_RGB(0,0,255), -1);

            if(_expClass->HMoving() < 0)
                cvPutText( _outputImage, "HORIZONTAL: LEFT", cvPoint(10, 20), &_font, CV_RGB(255,255,255) );
            else if(_expClass->HMoving() > 0)
                cvPutText( _outputImage, "HORIZONTAL: RIGHT", cvPoint(10, 20), &_font, CV_RGB(255,255,255) );
            else
                cvPutText( _outputImage, "HORIZONTAL: FORWARD", cvPoint(10, 20), &_font, CV_RGB(255,255,255) );

            if(_expClass->VMoving() < 0)
                cvPutText( _outputImage, "VERTICAL:    UP", cvPoint(10, 40), &_font, CV_RGB(255,255,255) );
            else if(_expClass->VMoving() > 0)
                cvPutText( _outputImage, "VERTICAL:    DOWN", cvPoint(10, 40), &_font, CV_RGB(255,255,255) );
            else
                cvPutText( _outputImage, "VERTICAL:    FORWARD", cvPoint(10, 40), &_font, CV_RGB(255,255,255) );
        }
    }

	// Mouth AAM stat
	if( ( _iFaceOpt & AAM_FIT_MOUTH ) && ( pTracking & AAM_FIT_MOUTH ) && _ff->FaceRect() && _ff->MouthRect() )
	{
		_mouthAAM->CreateStat( _outputImage, 1, true, CV_RGB(0,0,255) );

		// Mouth opening/closing stat
		if( ( _expClassOpt & MOUTH_OPEN_CLOSE ) && ( pExpression & MOUTH_OPEN_CLOSE ) )
		{
			char textArray[1000] = { '\0' };

			_expClass->MouthStatusThreshold(0.38);
			sprintf( textArray, "%s", _expClass->IsMouthOpen() ? "Open" : "Close" );
			cvPutText( _outputImage, textArray, cvPoint(_ff->MouthRect()->x, _ff->MouthRect()->y - 5), &_font, CV_RGB(255,255,255) );
		}
	}

	// Gaze direction stat
	//if(( _expClassOpt & ESTIMATE_GAZE_DIRECTION ) && ( pExpression & ESTIMATE_GAZE_DIRECTION ) && _ff->FaceRect() && _ff->LeftEyeRect() && _ff->RightEyeRect() && _ff->NoseRect() && _ff->MouthRect())
	//{
	//	cvCircle(_outputImage, _expClass->Gaze(), 3, CV_RGB(0,0,255), -1);
	//	cvLine(_outputImage, _expClass->FeatureCenter(), _expClass->Gaze(), CV_RGB(0,0,255), 1, CV_AA);
	//	cvCircle(_outputImage, _expClass->FeatureCenter(), 3, CV_RGB(0,0,255), -1);

	//	if(_expClass->HMoving() < 0)
	//		cvPutText( _outputImage, "HORIZONTAL: LEFT", cvPoint(10, 20), &_font, CV_RGB(255,255,255) );
	//	else if(_expClass->HMoving() > 0)
	//		cvPutText( _outputImage, "HORIZONTAL: RIGHT", cvPoint(10, 20), &_font, CV_RGB(255,255,255) );
	//	else
	//		cvPutText( _outputImage, "HORIZONTAL: FORWARD", cvPoint(10, 20), &_font, CV_RGB(255,255,255) );

	//	if(_expClass->VMoving() < 0)
	//		cvPutText( _outputImage, "VERTICAL:    UP", cvPoint(10, 40), &_font, CV_RGB(255,255,255) );
	//	else if(_expClass->VMoving() > 0)
	//		cvPutText( _outputImage, "VERTICAL:    DOWN", cvPoint(10, 40), &_font, CV_RGB(255,255,255) );
	//	else
	//		cvPutText( _outputImage, "VERTICAL:    FORWARD", cvPoint(10, 40), &_font, CV_RGB(255,255,255) );
	//}

	// Left eye blinking (skin color)
	if( ( _expClassOpt & BLINKING_SKIN_COLOR ) && ( pExpression & BLINKING_SKIN_COLOR ) && _ff->FaceRect() && _ff->LeftEyeRect() && _expClass->LeftEyeBlinkRateSC() != -1 )
	{
		if( _expClass->VMoving() == 0 && _expClass->HMoving() == 0 && _expClass->LeftEyeBlinkRateSC() < 16 )
			cvPutText( _outputImage, "Blinking", cvPoint(_ff->LeftEyeRect()->x, _ff->LeftEyeRect()->y - 5), &_font, CV_RGB(255,255,255) );
	}

	// Right eye blinking (skin color)
	if( ( _expClassOpt & BLINKING_SKIN_COLOR ) && ( pExpression & BLINKING_SKIN_COLOR ) && _ff->FaceRect() && _ff->RightEyeRect() && _expClass->RightEyeBlinkRateSC() != -1 )
	{
		if( _expClass->VMoving() == 0 && _expClass->HMoving() == 0 && _expClass->RightEyeBlinkRateSC() < 16 )
			cvPutText( _outputImage, "Blinking", cvPoint(_ff->RightEyeRect()->x, _ff->RightEyeRect()->y - 5), &_font, CV_RGB(255,255,255) );
	}

	_ff->CreateStat( _outputImage, CV_RGB(255,0,0) );
}

void IFaceTracker::CreateMessage(int pTracking, int pExpression)
{
    stringstream ss;
    CvRect *r = _ff->FaceRect();

    ss.str("");

    if(r)
    {
        // Face and facial features stat
        ss << "#FACERECT" << r->x << "|" << r->y << "|" << r->width << "|" << r->height;

        r = _ff->NoseRect();
        if( r )
            ss << "#NOSERECT" << r->x << "|" << r->y << "|" << r->width << "|" << r->height;

        r = _ff->LeftEyeRect();
        if( r )
            ss << "#LEFTEYERECT" << r->x << "|" << r->y << "|" << r->width << "|" << r->height;

        r = _ff->RightEyeRect();
        if( r )
            ss << "#RIGHTEYERECT" << r->x << "|" << r->y << "|" << r->width << "|" << r->height;

        r = _ff->MouthRect();
        if( r )
        {
            ss << "#MOUTHRECT" << r->x << "|" << r->y << "|" << r->width << "|" << r->height;

            // Mouth AAM stat
            if( ( _iFaceOpt & AAM_FIT_MOUTH ) && ( pTracking & AAM_FIT_MOUTH ) )
            {
                ss << "#MOUTHAAM";

                vector<CvPoint2D32f> p = _mouthAAM->ImagePoints();

                for(int i = 0; i < (int)p.size(); i++)
                    ss << "|" << p[i].x << "|" << p[i].y;

                // Mouth opening/closing stat
                if( ( _expClassOpt & MOUTH_OPEN_CLOSE ) && ( pExpression & MOUTH_OPEN_CLOSE ) )
                    ss << "#MOUTHSTATE" << _expClass->MouthAxisRate();
            }
        }

        // Face AAM stat
        if( ( _iFaceOpt & AAM_FIT_FACE ) && ( pTracking & AAM_FIT_FACE ) )
        {
            ss << "#FACEAAM";

            vector<CvPoint2D32f> p = _faceAAM->ImagePoints();

            for(int i = 0; i < (int)p.size(); i++)
                ss << "|" << p[i].x << "|" << p[i].y;

            // Gaze direction stat
            if(( _expClassOpt & ESTIMATE_GAZE_DIRECTION ) && ( pExpression & ESTIMATE_GAZE_DIRECTION ))
                ss << "#GAZE" << _expClass->Gaze().x << "|" << _expClass->Gaze().y << "|" << _expClass->HMoving() << "|" << _expClass->VMoving();

            // Head Pose
            if(( _iFaceOpt & ESTIMATE_HEAD_POSE ) && ( pExpression & ESTIMATE_HEAD_POSE ))
            {
                CvPoint3D64f a = _headPose->Angles();
                CvVect32f t = _headPose->TranslationVector();
                ss << "#HEADPOSE" << a.x << "|" << a.y << "|" << a.z << "|" << t[0] << "|" << t[1] << "|" << t[2];
            }
        }
    }

    ss << "#";

    _message = ss.str();
}

IplImage* IFaceTracker::OutputImage( void ) const
{
	return _outputImage;
}

string IFaceTracker::Message(void) const 
{ 
    return _message; 
}
