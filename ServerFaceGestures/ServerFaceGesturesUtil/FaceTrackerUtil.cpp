#include "FaceTrackerUtil.h"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <stack>

/* ******************************* FaceTrackerUtil ******************************* */
FaceTrackerUtil::FaceTrackerUtil( void )
{
}

FaceTrackerUtil::~FaceTrackerUtil( void )
{
}

void FaceTrackerUtil::ShowImage( CvArr *pImg, const char *pName, int pIsCamera )
{
	cvNamedWindow( pName, CV_WINDOW_AUTOSIZE );
	cvShowImage( pName, pImg );

	if( !pIsCamera )
	{
		cvWaitKey( 0 );
		cvDestroyWindow( pName );
	}
}

uchar FaceTrackerUtil::GetPixel8U( IplImage *pImg, int pX, int pY )
{
	return ( (uchar *)( pImg->imageData + pY * pImg->widthStep ) )[ pX ];
}

void FaceTrackerUtil::SetPixel8U( IplImage *pImg, int pX, int pY, uchar pColor )
{
	( (uchar *)( pImg->imageData + pY * pImg->widthStep ) )[ pX ] = pColor;
}

void FaceTrackerUtil::UpdateBC( IplImage *pSrc, IplImage *pDst, int pNB, int pNC )
{
	uchar lut[256];
	CvMat *lut_mat;
	int brightness = pNB - 100;
	int contrast = pNC - 100;

	lut_mat = cvCreateMatHeader( 1, 256, CV_8UC1 );
	cvSetData( lut_mat, lut, 0 );

	if( contrast > 0 )
	{
		double delta = 127.0 * contrast / 100.0;
		double a = 255.0 / ( 255.0 - delta * 2.0 );
		double b = a * ( brightness - delta );

		for( int i = 0; i < 256; i++ )
		{
			int v = cvRound( a * i + b );

			if( v < 0 )
				v = 0;

			if( v > 255 )
				v = 255;

			lut[i] = (uchar)v;
		}
	}
	else
	{
		double delta = -128.0 * contrast / 100.0;
		double a = ( 256.0 - delta * 2.0 ) / 255.0;
		double b = a * brightness + delta;

		for( int i = 0; i < 256; i++ )
		{
			int v = cvRound( a * i + b );
			if( v < 0 )
				v = 0;

			if( v > 255 )
				v = 255;

			lut[i] = (uchar)v;
		}
	}

	cvLUT( pSrc, pDst, lut_mat );
}

void FaceTrackerUtil::ColorStrEqualize( IplImage *pSrc, IplImage *pDst, int pType )
{
	IplImage *r = cvCreateImage( cvGetSize( pSrc ), 8, 1 );
	IplImage *g = cvCreateImage( cvGetSize( pSrc ), 8, 1 );
	IplImage *b = cvCreateImage( cvGetSize( pSrc ), 8, 1 );
	double rmin, rmax, gmin, gmax, bmin, bmax, tr, tg, tb;

	cvSplit( pSrc, b, g, r, NULL );	

	cvMinMaxLoc( r, &rmin, &rmax );
	cvMinMaxLoc( g, &gmin, &gmax );
	cvMinMaxLoc( b, &bmin, &bmax );

	if( pType == 2 )
	{
		rmin = gmin = bmin = min( rmin, min( gmin, bmin ) );
		rmax = gmax = bmax = max( rmax, max( gmax, bmax ) );
	}

	tr = 255.0 / ( rmax - rmin );
	tg = 255.0 / ( gmax - gmin );
	tb = 255.0 / ( bmax - bmin );

	for( int j = 0; j < r->height; j++ )
	{
		for( int i = 0; i < r->width; i++ )
		{
			uchar px_r_color = FaceTrackerUtil::GetPixel8U( r, i, j );
			uchar px_g_color = FaceTrackerUtil::GetPixel8U( g, i, j );
			uchar px_b_color = FaceTrackerUtil::GetPixel8U( b, i, j );

			FaceTrackerUtil::SetPixel8U( r, i, j, (int)( ( px_r_color - rmin ) * tr ) );
			FaceTrackerUtil::SetPixel8U( g, i, j, (int)( ( px_g_color - gmin ) * tg ) );
			FaceTrackerUtil::SetPixel8U( b, i, j, (int)( ( px_b_color - bmin ) * tb ) );
		}
	}

	cvMerge( b, g, r, NULL, pDst );

	cvReleaseImage( &r );
	cvReleaseImage( &g );
	cvReleaseImage( &b );
}

void FaceTrackerUtil::ResizeConstProp( IplImage *pImg, IplImage **pSrc, int pX )
{
	double ratio = pImg->width > pImg->height ? (double)pX / pImg->width : (double)pX / pImg->height;
	double width, height;

	width = pImg->width * ratio;
	height = pImg->height * ratio;

	*pSrc = cvCreateImage( cvSize( (int)width, (int)height ), pImg->depth, pImg->nChannels );
	cvResize( pImg, *pSrc );
}

CvPoint2D32f FaceTrackerUtil::WindowToViewport( CvPoint2D32f pPoint, CvRect *pW, CvRect *pV )
{
	CvPoint2D32f projected;

	projected.x = ( ( pPoint.x - pW->x ) / ( pW->x + pW->width - pW->x ) ) * ( pV->x + pV->width - pV->x ) + pV->x;
	projected.y = ( ( pPoint.y - pW->y ) / ( pW->y + pW->height - pW->y ) ) * ( pV->y + pV->height - pV->y ) + pV->y;

	return projected;
}

void FaceTrackerUtil::CalcHist( IplImage *pImg, bool pDrawHist = false )
{
	int				hist_size = 256;
	int				sch = pImg->nChannels, ch_width = 260;
	float			max_val = 0;
	float			range_0[] = { 0, 256 };
	float*			ranges[] = { range_0 };
	IplImage*		planes[4] = { 0, 0, 0, 0 }, *hist_img = NULL;
	CvHistogram*	hist[4];
	float			avg_values[4] = {0.0, 0.0, 0.0};


	for( int i = 0; i < sch; i++ ) 
	{
		planes[i] = cvCreateImage(cvSize(pImg->width, pImg->height), pImg->depth, 1);
		hist[i] = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	}

	// (2) allocate a histogram image
	hist_img =		cvCreateImage( cvSize(ch_width * sch, 200), IPL_DEPTH_8U, 3 );
	if( sch == 1 ) 
	{
		// (3a)if the source image has single-channel, calculate its histogram
		cvCopy(pImg, planes[0], NULL);
		cvCalcHist (&planes[0], hist[0], 0, NULL);
		cvGetMinMaxHistValue (hist[0], 0, &max_val, 0, 0);
	} 
	else 
	{
		// (3b)if the souce image has multi-channel, aplit it and calculate histogram of each plane
		cvSplit(pImg, planes[0], planes[1], planes[2], planes[3]);

		for( int i = 0; i < sch; i++ ) 
		{
			float tmp_val;
			cvCalcHist(&planes[i], hist[i], 0, NULL);
			cvGetMinMaxHistValue(hist[i], 0, &tmp_val, 0, 0);
			max_val = max_val < tmp_val ? tmp_val : max_val;
		}
	}

	// (4)scale and draw the histogram(s)
	cvSet(hist_img, cvScalarAll(255), 0);

	for( int i = 0; i < sch; i++ ) 
	{
		int			bin_w;
		CvScalar	color = cvScalarAll(100);

		if( sch == 3 )
			color = cvScalar( (0xaa<<i*8)&0x0000ff, (0xaa<<i*8)&0x00ff00, (0xaa<<i*8)&0xff0000, 0 );

		cvScale(hist[i]->bins, hist[i]->bins, ((double) hist_img->height) / max_val, 0);
		bin_w = cvRound((double) ch_width / hist_size);

		for( int j = 0; j < hist_size; j++ )
		{
			cvRectangle(hist_img,
				cvPoint(j*bin_w+(i*ch_width), hist_img->height),
				cvPoint((j+1)*bin_w+(i*ch_width), hist_img->height-cvRound(cvGetReal1D(hist[i]->bins, j))),
				color, -1, 8, 0);

			avg_values[i] += cvRound(cvGetReal1D(hist[i]->bins, j));
		}
	}

	for( int i = 0; i < sch; i++ )
	{
		avg_values[i] /= hist_size;
	}

	// (5)show the histogram image
	if( pDrawHist )
	{
		FaceTrackerUtil::ShowImage(hist_img, "Histogram", 1);
	}

	cvReleaseImage(&hist_img);

	for(int i=0; i<sch; i++) 
	{
		cvReleaseImage(&planes[i]);
		cvReleaseHist(&hist[i]);
	}
}

/* ******************************* FaceTrackerUtil ******************************* */

/* ******************************* HaarDetector ******************************* */
HaarDetector::HaarDetector( void )
{

}

HaarDetector::HaarDetector( string pCascadeFileName )
{
	_storage = cvCreateMemStorage( 0 );
	//_cascade = (CvHaarClassifierCascade*)cvLoad( pCascadeFileName.c_str(), 0, 0, 0 ); // TODO: iPhone-ra átírni!
	_cascade = (CvHaarClassifierCascade*)cvLoad( pCascadeFileName.c_str(), 0, 0, 0 );
	_minSize = cvSize( 30, 30 );

	if( !_cascade )
	{
		cout << "Warning: cascade file does not exist (" << pCascadeFileName << ")!" << endl;
	}
	else
	{
		cout << "Reading cascade file (" << pCascadeFileName << ")... OK!" << endl;
	}
}

HaarDetector::~HaarDetector( void )
{
	if( _cascade )
	{
		cvFree( &_cascade );
		cvFree( &_storage );
	}
}

CvSeq* HaarDetector::Detect( IplImage* pImg )
{
	return cvHaarDetectObjects( pImg, _cascade, _storage,  1.1, 3, 0 | CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE, _minSize );
}

void HaarDetector::SetMinsize( CvSize pMinSize )
{
	_minSize = pMinSize;
}
/* ******************************* HaarDetector ******************************* */
