#include "AAM_TDM.h"
#include "AAM_Shape.h"

AAM_TDM::AAM_TDM()
{
	__MeanTexture = 0;
	__TextureEigenVectors = 0;
	__TextureEigenValues = 0; 

}

AAM_TDM::~AAM_TDM()
{
	cvReleaseMat(&__MeanTexture);
	cvReleaseMat(&__TextureEigenVectors);
	cvReleaseMat(&__TextureEigenValues);
}

//============================================================================
void AAM_TDM::CalcTexture(const CvMat* lamda, CvMat* texture)
{
	cvBackProjectPCA(lamda, __MeanTexture, __TextureEigenVectors, texture);
}

//============================================================================
void AAM_TDM::CalcParams(const CvMat* texture, CvMat* lamda)
{
	cvProjectPCA(texture, __MeanTexture, __TextureEigenVectors, lamda);
}

void AAM_TDM::Clamp(CvMat* lamda, double s_d /* = 3.0 */)
{
	double* fastp = lamda->data.db;
	double* fastv = __TextureEigenValues->data.db;
	int nmodes = nModes();
	double limit;

	for(int i = 0; i < nmodes; i++)
	{
		limit = s_d*sqrt(fastv[i]);
		if(fastp[i] > limit) fastp[i] = limit;
		else if(fastp[i] < -limit) fastp[i] = -limit;	
	}
}

//============================================================================
void AAM_TDM::AlignTextureToRef(const CvMat* refTextrure, CvMat* Texture)
{
	AAM_TDM::ZeroMeanUnitLength(Texture);
	double alpha = cvDotProduct(Texture, refTextrure);

	if(fabs(alpha) > DBL_EPSILON)	
		cvConvertScale(Texture, Texture, 1.0 / alpha, 0.0);
}

//============================================================================
void AAM_TDM::ZeroMeanUnitLength(CvMat* Texture)
{
	CvScalar mean =  cvAvg(Texture);
	cvSubS(Texture, mean, Texture);
	double norm = cvNorm(Texture);
	cvConvertScale(Texture, Texture, 1.0 / norm);
}

//============================================================================
void AAM_TDM::Read(std::ifstream& is)
{	
	int _npixels, _nModes;
	is >> _npixels >> _nModes;

	__MeanTexture = cvCreateMat(1, _npixels, CV_64FC1);
	__TextureEigenValues = cvCreateMat(1, _nModes, CV_64FC1);
	__TextureEigenVectors = cvCreateMat(_nModes, _npixels, CV_64FC1);

	is >> __MeanTexture;
	is >> __TextureEigenValues;
	is >> __TextureEigenVectors;
}