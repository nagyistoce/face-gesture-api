#pragma once

#ifndef _FACETRACKERUTIL_H_
#define _FACETRACKERUTIL_H_

#include "macros.h"

using namespace std;

/* ******************************* FaceTrackerUtil ******************************* */
class FACE_API FaceTrackerUtil
{
public:
    FaceTrackerUtil		( void );
    ~FaceTrackerUtil	( void );

	static void				UpdateBC			( IplImage *pSrc, IplImage *pDst, int pNB, int pNC );
	static void				ColorStrEqualize	( IplImage *pSrc, IplImage *pDst, int pType = 2 );
	static uchar			GetPixel8U			( IplImage *pImg, int pX, int pY );
	static void				SetPixel8U			( IplImage *pImg, int pX, int pY, uchar pColor );
	static void				ShowImage			( CvArr *pImg, const char *pName, int pIsCamera = 0 );
	static void				ResizeConstProp		( IplImage *pImg, IplImage **pSrc, int pX );
	static CvPoint2D32f		WindowToViewport	( CvPoint2D32f pPoint, CvRect *pW, CvRect *pV ); 
	static void				CalcHist			( IplImage *pImg, bool pDrawHist );
};
/* ******************************* FaceTrackerUtil ******************************* */

/* ******************************* HaarDetector ******************************* */
class FACE_API HaarDetector
{
public:
	HaarDetector	( void );
	HaarDetector	( string pCascadeFileName );
	~HaarDetector	( void );

	CvSeq*	Detect		( IplImage* pImg );
	void	SetMinsize	( CvSize pMinSize );

private:
	CvHaarClassifierCascade*	_cascade;
	CvMemStorage*				_storage;
	CvSize						_minSize;
};
/* ******************************* HaarDetector ******************************* */

#endif //_FACETRACKERUTIL_H_
