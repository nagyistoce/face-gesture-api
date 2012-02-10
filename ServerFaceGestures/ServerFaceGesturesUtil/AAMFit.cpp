#include "AAMFit.h"

AAMFit::AAMFit( void )
{
	_model = NULL;

	_w1 = 0.5;
	_w2 = 0.5;

	_prevImagePoints.clear();
	_smoothedImagePoints.clear();
}

AAMFit::~AAMFit( void )
{
	if( _model )
		delete _model;
}

AAM_Shape AAMFit::InitShape( const AAM_Shape& pMeanShape, CvRect *pR, int pType = 0 )
{
	AAM_Shape detShape;
	AAM_Shape startShape;

	detShape.resize(2);
	detShape[0].x = pR->x;
	detShape[0].y = pR->y;
	detShape[1].x = detShape[0].x + pR->width;
	detShape[1].y = detShape[0].y + pR->height;

	if( pType == AAM_FIT_FACE )
		AdjustFaceShape(detShape);
	else if( pType == AAM_FIT_MOUTH )
		AdjustMouthShape(detShape);

	AlignShape(startShape, detShape, pMeanShape);

	return startShape;
}

void AAMFit::AdjustFaceShape( AAM_Shape& pShape )
{
	// following are for aligning base shape to (Viola Jones or other) detector shape
	static const double heightShift = 0.15;  // shift height down by 15%
	static const double shrink      = 0.7;   // shrink size of VJ box by 25%

	double xMin = pShape[0].x;
	double yMin = pShape[0].y;
	double xMax = pShape[1].x;
	double yMax = pShape[1].y;

	double NewHeight = shrink * (yMax - yMin);
	double NewWidth = shrink * (xMax - xMin);
	double yMean = (yMin + yMax) / 2;
	double xMean = (xMin + xMax) / 2;

	yMean += heightShift * (yMax - yMin);    // move face down

	pShape[0].x = xMean - 0.5 * NewWidth;
	pShape[0].y = yMean - 0.5 * NewHeight;
	pShape[1].x = xMean + 0.5 * NewWidth;
	pShape[1].y = yMean + 0.5 * NewHeight;
}

void AAMFit::AdjustMouthShape( AAM_Shape& pShape )
{
	// following are for aligning base shape to (Viola Jones or other) detector shape
	static const double heightShift	 = 0.1;  // shift height down by 15%
	static const double vShrink      = 0.8;	// shrink vertical the VJ box by 30%
	static const double hShrink      = 1.0;		// shrink horizontal the VJ box by 30%

	double xMin = pShape[0].x;
	double yMin = pShape[0].y;
	double xMax = pShape[1].x;
	double yMax = pShape[1].y;

	double NewHeight = hShrink * (yMax - yMin);
	double NewWidth = vShrink * (xMax - xMin);
	double yMean = (yMin + yMax) / 2;
	double xMean = (xMin + xMax) / 2;

	yMean += heightShift * (yMax - yMin);    // move face down

	pShape[0].x = xMean - 0.5 * NewWidth;
	pShape[0].y = yMean - 0.5 * NewHeight;
	pShape[1].x = xMean + 0.5 * NewWidth;
	pShape[1].y = yMean + 0.5 * NewHeight;
}


void AAMFit::AlignShape( AAM_Shape& pStartShape, const AAM_Shape& pDetShape, const AAM_Shape& pMeanShape )
{
	AAM_Shape baseShape, alignedShape;
	baseShape.resize(2);
	alignedShape.resize(2);

	double meanCenter = (pMeanShape.MinY() + pMeanShape.MaxY()) / 2;
	baseShape[0].x = pMeanShape.MinX();
	baseShape[0].y = meanCenter;
	baseShape[1].x = pMeanShape.MaxX();
	baseShape[1].y = meanCenter;

	double yMean = (pDetShape[1].y + pDetShape[0].y)/2;
	alignedShape[0].x = pDetShape[0].x;
	alignedShape[0].y = yMean;
	alignedShape[1].x = pDetShape[1].x;
	alignedShape[1].y = yMean;

	double a, b, tx, ty;
	baseShape.AlignTransformation(alignedShape, a, b, tx, ty);
	pStartShape = pMeanShape;
	pStartShape.TransformPose(a, b, tx, ty);

}

void AAMFit::LoadModel( string pModelFileName )
{
	ifstream fs( pModelFileName.c_str() );
	int modelType;

	if( fs == 0 )
	{
		cerr << "Warning: AAM model file does not exist (" + pModelFileName + ")!" << endl;
		return;
	}

	fs >> modelType;

	if( modelType == 0 )
		_model = new AAM_Basic;
	else if( modelType == 1 )
		_model = new AAM_IC;
	else
		return;

	//read model from file
	_model->Read( fs, pModelFileName );
	fs.close();
}

AAM* AAMFit::Model( void ) const 
{ 
	return _model; 
}

void AAMFit::Fit( IplImage* pFrame, CvRect *pR, int pType )
{
	if( _model == NULL ) return;

	AAM_Shape currentShape = InitShape( _model->GetMeanShape(), pR, pType );

	_model->Fit(pFrame, currentShape, 30, false); // TODO: Valahol szivárog egy kis memória - nem mindig
	_imagePoints = currentShape.getPoints();
}


void AAMFit::SmoothImagePoints( void )
{
	if( _prevImagePoints.empty() )
	{
		for( int i = 0; i < _imagePoints.size(); i++ )
		{
			_prevImagePoints.push_back( _imagePoints[i] );
			_smoothedImagePoints.push_back( _imagePoints[i] );
		}

		return;
	}

	for( int i = 0; i < _imagePoints.size(); i++ )
	{
		_smoothedImagePoints[i].x = _w1 * _prevImagePoints[i].x + _w2 * _imagePoints[i].x;
		_prevImagePoints[i].x = _imagePoints[i].x;

		_smoothedImagePoints[i].y = _w1 * _prevImagePoints[i].y + _w2 * _imagePoints[i].y;
		_prevImagePoints[i].y = _imagePoints[i].y;
	}
}

vector<CvPoint2D32f> AAMFit::ImagePoints( void ) const 
{ 
	return _smoothedImagePoints.empty() ? _imagePoints : _smoothedImagePoints; 
}

void AAMFit::CreateStat( IplImage *pFrame, int pMode, bool pDrawNumbers, CvScalar pRGB )
{
	CvFont font;
	char text[20];

	cvInitFont( &font, CV_FONT_HERSHEY_SIMPLEX, 0.1, 1.0, 0, 1, CV_AA );
	//_model->Draw(pFrame, pMode);

	if( _smoothedImagePoints.empty() )
	{
		for( int i = 0; i < _imagePoints.size(); i++ )
		{
			cvCircle( pFrame, cvPoint( _imagePoints[i].x, _imagePoints[i].y), 3, pRGB, -1 );

			if( pDrawNumbers )
			{
				sprintf( text, "%d", i );
				cvPutText( pFrame, text, cvPoint(_imagePoints[i].x, _imagePoints[i].y), &font, cvScalar(255, 255, 255, 0) );
			}
		}
	}
	else
	{
		for( int i = 0; i < _smoothedImagePoints.size(); i++ )
		{
			cvCircle( pFrame, cvPoint( _smoothedImagePoints[i].x, _smoothedImagePoints[i].y), 3, pRGB, -1 );

			if( pDrawNumbers )
			{
				sprintf( text, "%d", i );
				cvPutText( pFrame, text, cvPoint(_smoothedImagePoints[i].x, _smoothedImagePoints[i].y), &font, cvScalar(255, 255, 255, 0) );
			}
		}
	}
}
