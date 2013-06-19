#pragma once

#include "AAM_TDM.h"
#include "AAM_PDM.h"
#include "AAM_PAW.h"

class AAM_Basic;

//! Combined appearance model
/*!
	\ingroup AAM
*/
extern "C++" class __declspec(dllexport) AAM_CAM  
{
	friend class AAM_Basic;
public:
	//! Constructor
	AAM_CAM(void);

	//! Destructor
	~AAM_CAM(void);

	//! Calculate the local shape
	void CalcLocalShape(CvMat* s, const CvMat* c);

	//! Calculate the global shape
	void CalcGlobalShape(CvMat* s, const CvMat* pose);
	
	//! calculate texture according to appearance parameters
	void CalcTexture(CvMat* t, const CvMat* c);
	
	//! Calculate combined appearance parameters from shape and texture params.
	void CalcParams(CvMat* c, const CvMat* bs, const CvMat* bg);

	//! Limit appearance parameters.
    void Clamp(CvMat* c, double s_d = 3.0);

	//! Read data from stream 
	void Read(std::ifstream& is);

	//! calculate shape according to appearance parameters
	inline void CalcShape(CvMat* s, const CvMat* c, const CvMat* pose){
		CalcLocalShape(s, c); 
		CalcGlobalShape(s, pose);	
	}

	//! Get number of modes of combined appearance variation
	inline const int nModes(void) const { 
		return __AppearanceEigenVectors->rows;
	}

private:
	AAM_PDM		__shape;		/*shape distribution model*/
	AAM_TDM		__texture;		/*shape distribution model*/
	AAM_PAW		__paw;			/*piecewise affine warp*/
	double      __WeightsS2T;   /*ratio between shape and texture model*/
	
	CvMat* __MeanAppearance;
	CvMat* __AppearanceEigenValues;
	CvMat* __AppearanceEigenVectors;
	
	CvMat* __Qs;
	CvMat* __Qg;
	CvMat* __MeanS;
	CvMat* __MeanG;

	//these cached variables are used for speed up
	CvMat*			__Points;
	CvMemStorage*	__Storage;
	CvMat*			__pq;

	CvMat* __a;
};
