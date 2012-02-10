#pragma warning( disable: 4251 ) /* needsto have dll-interface to be used by clients of class */

#include "HeadPose.h"
#include <iostream>
#include <cmath>

#define FOCAL_LENGTH 760.0f

HeadPose::HeadPose( void )
{
	CreateModelPoints();

    _positObject = cvCreatePOSITObject( &_modelPoints[0], (int)_modelPoints.size() );
    _rotationMatrix = new float[9];
    _translationVector = new float[3];
    _criteria = cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10000, 0.5f );

    for( int i = 0; i < 9; i++ )
        _rotationMatrix[i] = 0.0;

    for( int i = 0; i < 3; i++ )
        _translationVector[i] = 0.0;

    _distance = 0;
    _angles[0] = _angles[1] = _angles[2] = 0.0;
}

HeadPose::~HeadPose( void )
{
    delete _rotationMatrix;
    delete _translationVector;

    cvReleasePOSITObject( &_positObject );
}

void HeadPose::EstimateHeadPose( void )
{
    cvPOSIT( _positObject, &_imagePoints[0], FOCAL_LENGTH, _criteria, _rotationMatrix, _translationVector );

    for( size_t p = 0; p < _modelPoints.size(); p++ )
    {
		CvPoint3D32f point3D;
		
		point3D.x = _rotationMatrix[0] * _modelPoints[p].x + 
					_rotationMatrix[1] * _modelPoints[p].y +
					_rotationMatrix[2] * _modelPoints[p].z + _translationVector[0];

		point3D.y = _rotationMatrix[3] * _modelPoints[p].x + 
					_rotationMatrix[4] * _modelPoints[p].y +
					_rotationMatrix[5] * _modelPoints[p].z + _translationVector[1];

		point3D.z = _rotationMatrix[6] * _modelPoints[p].x + 
					_rotationMatrix[7] * _modelPoints[p].y +
					_rotationMatrix[8] * _modelPoints[p].z + _translationVector[2];
		
		CvPoint2D32f point2D = cvPoint2D32f( 0.0, 0.0 );
		
		if ( point3D.z != 0 )
		{
			point2D.x = (float)( FOCAL_LENGTH * point3D.x / point3D.z ); 
			point2D.y = (float)( FOCAL_LENGTH * point3D.y / point3D.z ); 	
		}
    }
}

void HeadPose::Dump( void )
{
    std::cout << "\n====================================\n";

    std::cout << "\n-.- MODEL POINTS -.-\n";  
    for( size_t p = 0; p < _modelPoints.size(); p++ )
        std::cout << _modelPoints[p].x << ", " << _modelPoints[p].y << ", " << _modelPoints[p].z << "\n";

    std::cout << "\n-.- IMAGE POINTS -.-\n";
    for( size_t p = 0; p < _imagePoints.size(); p++ )
        std::cout << _imagePoints[p].x << ", " << _imagePoints[p].y << " \n";

    std::cout << "\n-.- ESTIMATED ROTATION\n";
    for( size_t p = 0; p < 3; p++ )
        std::cout << _rotationMatrix[p*3] << " | " << _rotationMatrix[p*3+1] << " | " << _rotationMatrix[p*3+2] << "\n";

    std::cout << "\n-.- ESTIMATED TRANSLATION\n";
    std::cout << _translationVector[0] << " | " << _translationVector[1] << " | " << _translationVector[2] << "\n";
}

void HeadPose::DrawXYPlane( IplImage *pFrame, CvRect *pFaceRect )
{
	CvPoint3D32f viewPoint3D, modelViewPoint3D[32], rectCenter3D;
	CvPoint2D32f viewPoint2D[32];
	CvScalar s1 = CV_RGB(200,0,0), s2 = CV_RGB(100,0,0);
	CvRect viewport = cvRect( 0, 0, pFaceRect->width, pFaceRect->height );

	rectCenter3D.x = float( ( _modelPoints.at(33).x + _modelPoints.at(38).x + _modelPoints.at(6).x ) / 3.0 );
	rectCenter3D.y = float( ( _modelPoints.at(33).y + _modelPoints.at(38).y + _modelPoints.at(6).y ) / 3.0 );
	rectCenter3D.z = float( ( _modelPoints.at(33).z + _modelPoints.at(38).z + _modelPoints.at(6).z ) / 3.0 );

	modelViewPoint3D[0].x = rectCenter3D.x;
	modelViewPoint3D[0].y = rectCenter3D.y - 10;
	modelViewPoint3D[0].z = rectCenter3D.z - 10;

	modelViewPoint3D[1].x = rectCenter3D.x;
	modelViewPoint3D[1].y = rectCenter3D.y + 10;
	modelViewPoint3D[1].z = rectCenter3D.z - 10;

	modelViewPoint3D[2].x = rectCenter3D.x;
	modelViewPoint3D[2].y = rectCenter3D.y - 10;
	modelViewPoint3D[2].z = rectCenter3D.z + 10;

	modelViewPoint3D[3].x = rectCenter3D.x;
	modelViewPoint3D[3].y = rectCenter3D.y + 10;
	modelViewPoint3D[3].z = rectCenter3D.z + 10;

    for( int i = 4; i < 11; i++ )
    {
        modelViewPoint3D[i].x = modelViewPoint3D[0].x;
	    modelViewPoint3D[i].y = modelViewPoint3D[0].y + (i-3)*2.5f;
	    modelViewPoint3D[i].z = modelViewPoint3D[0].z;
    }

    for( int i = 11; i < 18; i++ )
    {
        modelViewPoint3D[i].x = modelViewPoint3D[2].x;
	    modelViewPoint3D[i].y = modelViewPoint3D[2].y + (i-10)*2.5f;
	    modelViewPoint3D[i].z = modelViewPoint3D[2].z;
    }

    for( int i = 18; i < 25; i++ )
    {
        modelViewPoint3D[i].x = modelViewPoint3D[0].x;
	    modelViewPoint3D[i].y = modelViewPoint3D[0].y;
	    modelViewPoint3D[i].z = modelViewPoint3D[0].z + (i-17)*2.5f;
    }

    for( int i = 25; i < 32; i++ )
    {
        modelViewPoint3D[i].x = modelViewPoint3D[1].x;
	    modelViewPoint3D[i].y = modelViewPoint3D[1].y;
	    modelViewPoint3D[i].z = modelViewPoint3D[1].z + (i-24)*2.5f;
    }

	for( int i = 0; i < 32; i++ )
	{
		viewPoint3D.x = _rotationMatrix[0] * modelViewPoint3D[i].x + 
					    _rotationMatrix[1] * modelViewPoint3D[i].y +
				        _rotationMatrix[2] * modelViewPoint3D[i].z + _translationVector[0];

		viewPoint3D.y = _rotationMatrix[3] * modelViewPoint3D[i].x + 
					    _rotationMatrix[4] * modelViewPoint3D[i].y +
					    _rotationMatrix[5] * modelViewPoint3D[i].z + _translationVector[1];
	
		viewPoint3D.z = _rotationMatrix[6] * modelViewPoint3D[i].x + 
					    _rotationMatrix[7] * modelViewPoint3D[i].y +
					    _rotationMatrix[8] * modelViewPoint3D[i].z + _translationVector[2];

		viewPoint2D[i] = cvPoint2D32f( 0.0, 0.0 );

		if( viewPoint3D.z != 0 )
		{
			viewPoint2D[i].x = (float)( FOCAL_LENGTH * viewPoint3D.x / viewPoint3D.z ); 
			viewPoint2D[i].y = (float)( FOCAL_LENGTH * viewPoint3D.y / viewPoint3D.z ); 	

            viewPoint2D[i] = FaceTrackerUtil::WindowToViewport( viewPoint2D[i], pFaceRect, &viewport );
			viewPoint2D[i] = FaceTrackerUtil::WindowToViewport( viewPoint2D[i], &viewport, pFaceRect );
		}
	}

    for( int i = 4; i < 11; i++ )
    {
        DisplayLine( pFrame, viewPoint2D[i], viewPoint2D[i+7], s2 );
        DisplayLine( pFrame, viewPoint2D[i+14], viewPoint2D[i+21], s2 );
    }

	/* Display sides */
    DisplayLine( pFrame, viewPoint2D[0], viewPoint2D[1], s1 );
    DisplayLine( pFrame, viewPoint2D[2], viewPoint2D[3], s1 );
    DisplayLine( pFrame, viewPoint2D[0], viewPoint2D[2], s1 );
    DisplayLine( pFrame, viewPoint2D[1], viewPoint2D[3], s1 );
}

void HeadPose::DrawXZPlane( IplImage *pFrame, CvRect *pFaceRect )
{
	CvPoint3D32f viewPoint3D, modelViewPoint3D[32], rectCenter3D;
	CvPoint2D32f viewPoint2D[32];
	CvScalar s1 = CV_RGB(0,200,0), s2 = CV_RGB(0,100,0);
	CvRect viewport = cvRect( 0, 0, pFaceRect->width, pFaceRect->height );

	rectCenter3D.x = float( ( _modelPoints.at(33).x + _modelPoints.at(38).x + _modelPoints.at(6).x ) / 3.0 );
	rectCenter3D.y = float( ( _modelPoints.at(33).y + _modelPoints.at(38).y + _modelPoints.at(6).y ) / 3.0 );
	rectCenter3D.z = float( ( _modelPoints.at(33).z + _modelPoints.at(38).z + _modelPoints.at(6).z ) / 3.0 );

	modelViewPoint3D[0].x = rectCenter3D.x - 10;
	modelViewPoint3D[0].y = rectCenter3D.y;
	modelViewPoint3D[0].z = rectCenter3D.z - 10;

	modelViewPoint3D[1].x = rectCenter3D.x + 10;
	modelViewPoint3D[1].y = rectCenter3D.y;
	modelViewPoint3D[1].z = rectCenter3D.z - 10;

	modelViewPoint3D[2].x = rectCenter3D.x - 10;
	modelViewPoint3D[2].y = rectCenter3D.y;
	modelViewPoint3D[2].z = rectCenter3D.z + 10;

	modelViewPoint3D[3].x = rectCenter3D.x + 10;
	modelViewPoint3D[3].y = rectCenter3D.y;
	modelViewPoint3D[3].z = rectCenter3D.z + 10;

    for( int i = 4; i < 11; i++ )
    {
        modelViewPoint3D[i].x = modelViewPoint3D[0].x + (i-3)*2.5f;
	    modelViewPoint3D[i].y = modelViewPoint3D[0].y;
	    modelViewPoint3D[i].z = modelViewPoint3D[0].z;
    }

    for( int i = 11; i < 18; i++ )
    {
        modelViewPoint3D[i].x = modelViewPoint3D[2].x + (i-10)*2.5f;
	    modelViewPoint3D[i].y = modelViewPoint3D[2].y;
	    modelViewPoint3D[i].z = modelViewPoint3D[2].z;
    }

    for( int i = 18; i < 25; i++ )
    {
        modelViewPoint3D[i].x = modelViewPoint3D[0].x;
	    modelViewPoint3D[i].y = modelViewPoint3D[0].y;
	    modelViewPoint3D[i].z = modelViewPoint3D[0].z + (i-17)*2.5f;
    }

    for( int i = 25; i < 32; i++ )
    {
        modelViewPoint3D[i].x = modelViewPoint3D[1].x;
	    modelViewPoint3D[i].y = modelViewPoint3D[1].y;
	    modelViewPoint3D[i].z = modelViewPoint3D[1].z + (i-24)*2.5f;
    }

	for( int i = 0; i < 32; i++ )
	{
		viewPoint3D.x = _rotationMatrix[0] * modelViewPoint3D[i].x + 
					    _rotationMatrix[1] * modelViewPoint3D[i].y +
				        _rotationMatrix[2] * modelViewPoint3D[i].z + _translationVector[0];

		viewPoint3D.y = _rotationMatrix[3] * modelViewPoint3D[i].x + 
					    _rotationMatrix[4] * modelViewPoint3D[i].y +
					    _rotationMatrix[5] * modelViewPoint3D[i].z + _translationVector[1];
	
		viewPoint3D.z = _rotationMatrix[6] * modelViewPoint3D[i].x + 
					    _rotationMatrix[7] * modelViewPoint3D[i].y +
					    _rotationMatrix[8] * modelViewPoint3D[i].z + _translationVector[2];

		viewPoint2D[i] = cvPoint2D32f( 0.0, 0.0 );

		if( viewPoint3D.z != 0 )
		{
			viewPoint2D[i].x = (float)( FOCAL_LENGTH * viewPoint3D.x / viewPoint3D.z ); 
			viewPoint2D[i].y = (float)( FOCAL_LENGTH * viewPoint3D.y / viewPoint3D.z ); 	

            viewPoint2D[i] = FaceTrackerUtil::WindowToViewport( viewPoint2D[i], pFaceRect, &viewport );
			viewPoint2D[i] = FaceTrackerUtil::WindowToViewport( viewPoint2D[i], &viewport, pFaceRect );
		}
	}

    for( int i = 4; i < 11; i++ )
    {
        DisplayLine( pFrame, viewPoint2D[i], viewPoint2D[i+7], s2 );
        DisplayLine( pFrame, viewPoint2D[i+14], viewPoint2D[i+21], s2 );
    }

	/* Display sides */
    DisplayLine( pFrame, viewPoint2D[0], viewPoint2D[1], s1 );
    DisplayLine( pFrame, viewPoint2D[2], viewPoint2D[3], s1 );
    DisplayLine( pFrame, viewPoint2D[0], viewPoint2D[2], s1 );
    DisplayLine( pFrame, viewPoint2D[1], viewPoint2D[3], s1 );
}

void HeadPose::DisplayLine( IplImage *pFrame, CvPoint2D32f pPt1, CvPoint2D32f pPt2, CvScalar pRgb )
{
    cvLine( pFrame, cvPoint( (int)pPt1.x, (int)pPt1.y ), cvPoint( (int)pPt2.x, (int)pPt2.y ), pRgb, 1, CV_AA );
}

void HeadPose::CreateStat( IplImage *pFrame, CvRect* pFaceRect )
{
    double rate = _translationVector[2] / 200.0;
    char text[1000] = {0};
    CvFont myFont;

    cvInitFont( &myFont, CV_FONT_HERSHEY_DUPLEX, 0.7, 1.0, 0.0, 1, CV_AA );

    this->DrawXYPlane( pFrame, pFaceRect );
	this->DrawXZPlane( pFrame, pFaceRect );

    cvRectangle( pFrame, cvPoint( 0, pFrame->height - 30 ), 
        cvPoint( pFrame->width, pFrame->height ), CV_RGB(200,200,200), -1 );

    cvRectangle( pFrame, cvPoint( 0, pFrame->height - 30 ), 
        cvPoint( (int)( rate * pFrame->width ), pFrame->height ), CV_RGB(255,0,0), -1 );

	_distance = cvRound( _translationVector[2] / 200.0 );

    float *a = Angles();

    sprintf( text, "Distance: %d, (%.2lf), (%.2lf), (%.2lf)", _distance, a[0], a[1], a[2] );
	cvPutText( pFrame, text, 
        cvPoint( 10, pFrame->height - 7 ), &myFont, CV_RGB( 0, 0, 0 ) );
}

void HeadPose::CreateModelPoints( void )
{
	_modelPoints.push_back(cvPoint3D32f( 0.000, 0.000, 0.000 )); // 0
	_modelPoints.push_back(cvPoint3D32f( 1.500, -2.750, 1.250 )); // 1
	_modelPoints.push_back(cvPoint3D32f( 3.000, -5.250, 2.500 )); // 2
	_modelPoints.push_back(cvPoint3D32f( 4.500, -7.500, 3.750 )); // 3
	_modelPoints.push_back(cvPoint3D32f( 6.000, -9.250, 5.000 )); // 4
	_modelPoints.push_back(cvPoint3D32f( 7.500, -10.500, 6.250 )); // 5
	_modelPoints.push_back(cvPoint3D32f( 9.500, -10.750, 7.500 )); // 6
	_modelPoints.push_back(cvPoint3D32f( 11.500, -10.500, 6.250 )); // 7
	_modelPoints.push_back(cvPoint3D32f( 13.000, -9.250, 5.000 )); // 8
	_modelPoints.push_back(cvPoint3D32f( 14.500, -7.500, 3.750 )); // 9
	_modelPoints.push_back(cvPoint3D32f( 16.000, -5.250, 2.500 )); // 10
	_modelPoints.push_back(cvPoint3D32f( 17.500, -2.750, 1.250 )); // 11
	_modelPoints.push_back(cvPoint3D32f( 19.000, 0.000, 0.000 )); // 12
	_modelPoints.push_back(cvPoint3D32f( 15.500, 8.750, 2.500 )); // 13
	_modelPoints.push_back(cvPoint3D32f( 14.500, 10.000, 2.500 )); // 14
	_modelPoints.push_back(cvPoint3D32f( 13.500, 10.250, 2.500 )); // 15
	_modelPoints.push_back(cvPoint3D32f( 12.500, 10.000, 2.500 )); // 16
	_modelPoints.push_back(cvPoint3D32f( 11.500, 8.750, 2.500 )); // 17
	_modelPoints.push_back(cvPoint3D32f( 12.500, 8.500, 2.500 )); // 18
	_modelPoints.push_back(cvPoint3D32f( 13.500, 8.250, 2.500 )); // 19
	_modelPoints.push_back(cvPoint3D32f( 14.500, 8.250, 2.500 )); // 20
	_modelPoints.push_back(cvPoint3D32f( 3.500, 8.750, 2.500 )); // 21
	_modelPoints.push_back(cvPoint3D32f( 4.500, 10.000, 2.500 )); // 22
	_modelPoints.push_back(cvPoint3D32f( 5.500, 10.250, 2.500 )); // 23
	_modelPoints.push_back(cvPoint3D32f( 6.500, 10.000, 2.500 )); // 24
	_modelPoints.push_back(cvPoint3D32f( 7.500, 8.750, 2.500 )); // 25
	_modelPoints.push_back(cvPoint3D32f( 6.500, 8.500, 2.500 )); // 26
	_modelPoints.push_back(cvPoint3D32f( 5.500, 8.250, 2.500 )); // 27
	_modelPoints.push_back(cvPoint3D32f( 4.500, 8.250, 2.500 )); // 28
	_modelPoints.push_back(cvPoint3D32f( 11.500, 11.250, 7.500 )); // 29
	_modelPoints.push_back(cvPoint3D32f( 13.500, 12.750, 7.500 )); // 30
	_modelPoints.push_back(cvPoint3D32f( 15.500, 12.500, 7.500 )); // 31
	_modelPoints.push_back(cvPoint3D32f( 17.000, 11.500, 6.250 )); // 32
	_modelPoints.push_back(cvPoint3D32f( 18.000, 9.250, 5.000 )); // 33
	_modelPoints.push_back(cvPoint3D32f( 7.500, 11.250, 7.500 )); // 34
	_modelPoints.push_back(cvPoint3D32f( 5.500, 12.750, 7.500 )); // 35
	_modelPoints.push_back(cvPoint3D32f( 3.500, 12.500, 7.500 )); // 36
	_modelPoints.push_back(cvPoint3D32f( 2.000, 11.500, 6.250 )); // 37
	_modelPoints.push_back(cvPoint3D32f( 1.000, 9.250, 5.000 )); // 38
	_modelPoints.push_back(cvPoint3D32f( 6.250, -2.000, 5.000 )); // 39
	_modelPoints.push_back(cvPoint3D32f( 8.500, -0.500, 6.250 )); // 40
	_modelPoints.push_back(cvPoint3D32f( 9.500, -0.750, 7.500 )); // 41
	_modelPoints.push_back(cvPoint3D32f( 10.500, -0.500, 6.250 )); // 42
	_modelPoints.push_back(cvPoint3D32f( 12.750, -2.000, 5.000 )); // 43
	_modelPoints.push_back(cvPoint3D32f( 10.500, -3.000, 6.250 )); // 44
	_modelPoints.push_back(cvPoint3D32f( 9.500, -2.750, 7.500 )); // 45
	_modelPoints.push_back(cvPoint3D32f( 8.500, -3.000, 6.250 )); // 46
	_modelPoints.push_back(cvPoint3D32f( 9.000, 8.500, 6.250 )); // 47
	_modelPoints.push_back(cvPoint3D32f( 8.000, 5.500, 6.250 )); // 48
	_modelPoints.push_back(cvPoint3D32f( 7.250, 4.500, 6.250 )); // 49
	_modelPoints.push_back(cvPoint3D32f( 7.500, 2.500, 6.250 )); // 50
	_modelPoints.push_back(cvPoint3D32f( 8.500, 2.250, 6.250 )); // 51
	_modelPoints.push_back(cvPoint3D32f( 9.500, 2.000, 6.250 )); // 52
	_modelPoints.push_back(cvPoint3D32f( 10.500, 2.250, 6.250 )); // 53
	_modelPoints.push_back(cvPoint3D32f( 11.500, 2.500, 6.250 )); // 54
	_modelPoints.push_back(cvPoint3D32f( 11.750, 4.500, 6.250 )); // 55
	_modelPoints.push_back(cvPoint3D32f( 11.000, 5.500, 6.250 )); // 56
	_modelPoints.push_back(cvPoint3D32f( 10.000, 8.500, 6.250 )); // 57
}

int HeadPose::Distance(void) const 
{ 
    return _distance; 
}

CvMatr32f HeadPose::RotationMatrix(void) const 
{ 
    return _rotationMatrix; 
}

float* HeadPose::Angles(void)
{
    //_angles[0] = (float)atan2( _rotationMatrix[2], _rotationMatrix[8] ) * 180.0 / M_PI;
    //_angles[1] = (float)asin( -1.0 * _rotationMatrix[5] ) * 180.0 / M_PI;
    //_angles[2] = (float)atan2( _rotationMatrix[3], _rotationMatrix[4] ) * 180.0 / M_PI;

    // TODO: decomposeProjectionMatrix

    return _angles;
}

CvVect32f HeadPose::TranslationVector(void) const 
{ 
    return _translationVector; 
}

void HeadPose::ImagePoints(vector<CvPoint2D32f> pVal) 
{ 
    _imagePoints = pVal; 
}
