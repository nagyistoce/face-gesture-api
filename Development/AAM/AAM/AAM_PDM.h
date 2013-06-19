#pragma once

#include "opencv2/opencv.hpp"

#include "AAM_Shape.h"

class AAM_CAM;

//! 2D point distribution model.
/*!
	\ingroup AAM
*/
extern "C++" class __declspec(dllexport) AAM_PDM  
{
	friend class AAM_CAM;
public:
	//! Constructor
	AAM_PDM(void);

	//! Destructor
	~AAM_PDM(void);
	
	//! Read data from stream 
	void Read(std::ifstream& is);

	//! Calculate shape according to parameters p and q
	void CalcShape(const CvMat* pq, CvMat* s);

	//! Calculate parameters p and q according to shape 
	void CalcParams(const CvMat* s, CvMat* p, CvMat* q);

	//! Calculate parameters p and q according to shape 
	void CalcParams(const CvMat* s, CvMat* pq);

	//! Calculate parameters p and q according to shape 
	void CalcParams(const AAM_Shape& shape, CvMat* pq);

	//! Limit shape parameters.
    void Clamp(CvMat* p, double s_d = 3.0);
		
	//! Get number of points in shape model
	inline const int nPoints(void) const {
		return __MeanShape->cols / 2;
	}

	//! Get number of modes of shape variation
	inline const int nModes(void) const {
		return __ShapesEigenVectors->rows;
	}

	//! Get AAM reference shape (Maybe NOT Central)
	inline const AAM_Shape GetAAMReferenceShape(void) const { 
		return __AAMRefShape;	
	}

private:
	CvMat*		__MeanShape;
	CvMat*		__ShapesEigenVectors;
    CvMat*		__ShapesEigenValues; 
	CvMat*		__matshape;

	AAM_Shape	__AAMRefShape; 
	AAM_Shape   __x;
	AAM_Shape   __y;
};
