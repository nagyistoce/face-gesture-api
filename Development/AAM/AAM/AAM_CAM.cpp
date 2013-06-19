#include "AAM_CAM.h"

typedef unsigned char byte;

AAM_CAM::AAM_CAM(void)
{
	__MeanAppearance = 0;
	__AppearanceEigenValues = 0;
	__AppearanceEigenVectors = 0;
	__Qs = 0;
	__Qg = 0;
	__MeanS = 0;
	__MeanG = 0;

	__Points = 0;
	__Storage = 0;
	__pq = 0;

	__a = 0;
}

AAM_CAM::~AAM_CAM(void)
{
	cvReleaseMat(&__MeanAppearance);
	cvReleaseMat(&__AppearanceEigenValues);
	cvReleaseMat(&__AppearanceEigenVectors);
	cvReleaseMat(&__Qg);
	cvReleaseMat(&__Qs);
	cvReleaseMat(&__MeanS);
	cvReleaseMat(&__MeanG);

	cvReleaseMat(&__Points);
	cvReleaseMemStorage(&__Storage);
	cvReleaseMat(&__pq);

	cvReleaseMat(&__a);
}


//============================================================================
void AAM_CAM::CalcLocalShape(CvMat* s, const CvMat* c)
{
	cvMatMul(c, __Qs, s);
	cvAdd(s, __MeanS, s);	
}

//============================================================================
void AAM_CAM::CalcGlobalShape(CvMat* s, const CvMat* pose)
{
	int npoints = s->cols/2;
	double* fasts = s->data.db;
	double a = cvmGet(pose,0,0)+1, b = cvmGet(pose,0,1); 
	double tx = cvmGet(pose,0,2), ty = cvmGet(pose,0,3);
	double x, y;

	for(int i = 0; i < npoints; i++)
	{
		x = fasts[2*i  ];
		y = fasts[2*i+1];

		fasts[2*i  ] = a*x-b*y+tx;
		fasts[2*i+1] = b*x+a*y+ty;
	}
}

//============================================================================
void AAM_CAM::CalcTexture(CvMat* t, const CvMat* c)
{
	cvMatMul(c, __Qg, t);
	cvAdd(t, __MeanG, t);
}

//============================================================================
void AAM_CAM::CalcParams(CvMat* c, const CvMat* bs, const CvMat* bg)
{
	double* fasta = __a->data.db;
	double* fastbs = bs->data.db;
	double* fastbg = bg->data.db;

	int i;
	for(i = 0; i < bs->cols; i++)	fasta[i] = __WeightsS2T * fastbs[i];
	for(i = 0; i < bg->cols; i++)   fasta[i+bs->cols] = fastbg[i];

	cvProjectPCA(__a, __MeanAppearance, __AppearanceEigenVectors, c);
}

//============================================================================
void AAM_CAM::Clamp(CvMat* c, double s_d /* = 3.0 */)
{
	double* fastc = c->data.db;
	double* fastv = __AppearanceEigenValues->data.db;
	int nmodes = nModes();
	double limit;

	for(int i = 0; i < nmodes; i++)
	{
		limit = s_d*sqrt(fastv[i]);
		if(fastc[i] > limit) fastc[i] = limit;
		else if(fastc[i] < -limit) fastc[i] = -limit;	
	}
}

//============================================================================
void AAM_CAM::Read(std::ifstream& is)
{
	__shape.Read(is);
	__texture.Read(is);
	__paw.Read(is);

	int np, nfeatures;
	is >> np >> nfeatures >> __WeightsS2T;

	__MeanAppearance = cvCreateMat(1, nfeatures, CV_64FC1);
	__AppearanceEigenValues = cvCreateMat(1, np, CV_64FC1);
	__AppearanceEigenVectors = cvCreateMat(np, nfeatures, CV_64FC1);
	__Qs = cvCreateMat(np, __shape.nPoints()*2, CV_64FC1);
	__Qg = cvCreateMat(np, __texture.nPixels(), CV_64FC1);
	__MeanS = cvCreateMat(1, __shape.nPoints()*2, CV_64FC1);
	__MeanG = cvCreateMat(1, __texture.nPixels(), CV_64FC1);

	is >> __MeanAppearance;
	is >> __AppearanceEigenValues;
	is >> __AppearanceEigenVectors;
	is >> __Qs;
	is >> __Qg;
	is >> __MeanS;
	is >> __MeanG;

	__Points = cvCreateMat (1, __shape.nPoints(), CV_64FC2);
	__Storage = cvCreateMemStorage(0);
	__pq = cvCreateMat(1, __shape.nModes()+4, CV_64FC1);
	__a = cvCreateMat(1, __AppearanceEigenVectors->cols, CV_64FC1);
}
