/****************************************************************************
* 
* Copyright (c) 2008 by Yao Wei, all rights reserved.
*
* Author:      	Yao Wei
* Contact:     	njustyw@gmail.com
* 
* This software is partly based on the following open source: 
*  
*		- OpenCV 
* 
****************************************************************************/

#include "AAM_PDM.h"
using namespace std;

AAM_PDM::AAM_PDM()
{
	__MeanShape = 0;
	__ShapesEigenVectors = 0;
    __ShapesEigenValues = 0; 
	__matshape = 0;
}

AAM_PDM::~AAM_PDM()
{
	cvReleaseMat(&__MeanShape);
	cvReleaseMat(&__ShapesEigenVectors);
	cvReleaseMat(&__ShapesEigenValues);
	cvReleaseMat(&__matshape);
}

void AAM_PDM::CalcShape(const CvMat* pq, CvMat* s)
{
	int nmodes = nModes(), npoints = nPoints();
//	assert(pq->cols == 4+nmodes && s->cols == npoints*2);

	CvMat p;	cvGetCols(pq, &p, 4, 4+nmodes);
	cvBackProjectPCA(&p, __MeanShape, __ShapesEigenVectors,	s);
	
	double* fasts = s->data.db;
	double a=cvmGet(pq,0,0)+1, b=cvmGet(pq,0,1), 
		tx=cvmGet(pq,0,2), ty=cvmGet(pq,0,3);
	double x, y;
	for(int i = 0; i < npoints; i++)
	{
		x = fasts[2*i  ];
		y = fasts[2*i+1];
		
		fasts[2*i  ] = a*x-b*y+tx;
		fasts[2*i+1] = b*x+a*y+ty;
	}
}

void AAM_PDM::CalcParams(const CvMat* s, CvMat* pq)
{
	__x.Mat2Point(s);

	CalcParams(__x, pq);
}

void AAM_PDM::CalcParams(const CvMat* s, CvMat* p, CvMat* q)
{
	int nmodes = nModes(), npoints = nPoints();
//	assert(pq->cols == 4+nmodes);
	
	double a, b, tx, ty;
	double a_, b_, tx_, ty_;
	double norm;
	
	__y.Mat2Point(s);
	__y.COG(tx, ty);
	__y.Translate(-tx, -ty);
	cvmSet(q,0,2,tx);
	cvmSet(q,0,3,ty);
	
	// do a few iterations to get (s, theta, p)
	cvZero(p);
	for(int iter = 0; iter < 2; iter++)
	{
		cvBackProjectPCA(p, __MeanShape, __ShapesEigenVectors,	__matshape);
		__x.Mat2Point(__matshape);

		__x.AlignTransformation(__y, a, b, tx, ty); //in fact, tx = ty = 0

		norm = a*a + b*b;
		a_ = a / norm; b_ = -b / norm;
		tx_ = (-a*tx - b*ty) / norm; ty_ = (b*tx - a*ty) / norm;
		__x = __y;
		__x.TransformPose(a_, b_, tx_, ty_);

		__x.Point2Mat(__matshape);
		cvProjectPCA(__matshape, __MeanShape, __ShapesEigenVectors, p);
	}

	cvmSet(q, 0, 0, a -1);
	cvmSet(q, 0, 1, b);
	Clamp(p, 1.8);
}

void AAM_PDM::CalcParams(const AAM_Shape& shape, CvMat* pq)
{
	int nmodes = nModes(), npoints = nPoints();
//	assert(pq->cols == 4+nmodes);
	
	double a, b, tx, ty;
	double a_, b_, tx_, ty_;
	double norm;
	
	__y = shape;
	__y.COG(tx, ty);
	__y.Translate(-tx, -ty);
	cvmSet(pq,0,2,tx);
	cvmSet(pq,0,3,ty);
	
	// do a few iterations to get (s, theta, p)
	CvMat p;	cvGetCols(pq, &p, 4, 4+nmodes);
	cvZero(&p);
	for(int iter = 0; iter < 2; iter++)
	{
		cvBackProjectPCA(&p, __MeanShape, __ShapesEigenVectors,	__matshape);
		__x.Mat2Point(__matshape);

		__x.AlignTransformation(__y, a, b, tx, ty); //in fact, tx = ty = 0

		norm = a*a + b*b;
		a_ = a / norm; b_ = -b / norm;
		tx_ = (-a*tx - b*ty) / norm; ty_ = (b*tx - a*ty) / norm;
		__x = __y;
		__x.TransformPose(a_, b_, tx_, ty_);

		__x.Point2Mat(__matshape);
		cvProjectPCA(__matshape, __MeanShape, __ShapesEigenVectors, &p);
	}

	cvmSet(pq, 0, 0, a -1);
	cvmSet(pq, 0, 1, b);
	Clamp(&p, 1.8);
}

void AAM_PDM::Clamp(CvMat* p, double s_d /* = 3.0 */)
{
	double* fastp = p->data.db;
	double* fastv = __ShapesEigenValues->data.db;
	int nmodes = nModes();
	double limit;

	for(int i = 0; i < nmodes; i++)
	{
		limit = s_d*sqrt(fastv[i]);
		if(fastp[i] > limit) fastp[i] = limit;
		else if(fastp[i] < -limit) fastp[i] = -limit;	
	}
}

void AAM_PDM::Read(std::ifstream& is)
{
	int _nPoints, _nModes;
	is >> _nPoints >> _nModes;

	__MeanShape = cvCreateMat(1, _nPoints*2, CV_64FC1);
	__ShapesEigenValues = cvCreateMat(1, _nModes, CV_64FC1);
	__ShapesEigenVectors = cvCreateMat(_nModes, _nPoints*2, CV_64FC1);
	__AAMRefShape.resize(_nPoints);

	is >> __MeanShape; 
	is >> __ShapesEigenValues;
	is >> __ShapesEigenVectors;
	__AAMRefShape.Read(is);

	__matshape  = cvCreateMat(1, nPoints()*2, CV_64FC1);
}