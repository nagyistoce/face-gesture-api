#pragma once

#include "AAM_PDM.h"
#include "AAM_TDM.h"
#include "AAM_PAW.h"

//! Active appearace model using project-out inverse compositional fitting method.
/*!
	\ingroup AAM

	Refer to: I. Matthews and S. Baker. "Active Appearance Models Revisited". 2004
*/
extern "C++" class __declspec(dllexport) AAM_IC : public AAM 
{
public:
	//! Constructor
	AAM_IC(const std::string& type);

	//! Destructor
	~AAM_IC(void);

	//! Fit the image using Inverse Compositional. 
	virtual void Fit(const IplImage* image, AAM_Shape& Shape, int max_iter = 30, bool showprocess = false);

	//! Set all search parameters to zero
	void SetAllParamsZero(void);

	//! Draw the image according the searching result(0:point, 1:triangle, 2:appearance)
	virtual void Draw(IplImage* image, int type = 0);

	//! Read data from stream 
	virtual void Read(std::ifstream& is, std::string& pModelFileName);

	//! Get Mean Shape of IC model
	inline const AAM_Shape GetMeanShape(void) const { 
		return __sMean;	
	}

private:
	//! Draw image with points
	void DrawPoint(IplImage* image, bool drawNumbers = false);

	//! Draw image with triangle mesh
	void DrawTriangle(IplImage* image);

	//! Draw image with full appearance
	void DrawAppearance(IplImage* image);

	//! Inverse compose current warp with shape parameter update.
	/*
		Update warp N.W(x;p,q)<-- N.W(x;p,q) . N.W(x;delta_p,delta_q)^-1.
	*/
	void InverseCompose(const CvMat* dpq, const CvMat* s, CvMat* NewS);

	//! is the current shape within the image boundary?
	static void CheckShape(const CvMat* s, int w, int h);

	//these variables are used for train PAW
	CvMat*			__Points;
	CvMemStorage*	__Storage;

	AAM_PDM		__shape;		/*shape distribution model*/
	AAM_TDM		__texture;		/*shape distribution model*/
	AAM_PAW		__paw;			/*piecewise affine warp*/
	AAM_Shape	__sMean;		/*mean shape of model*/
	AAM_Shape	__sStar1, __sStar2, __sStar3, __sStar4;/*global shape transform bases*/ 
	CvMat*		__G;			/*Update matrix*/
					/*product of inverse Hessian with steepest descent image*/

	//pre-allocated stuff for online alignment
	CvMat*		__update_s0;	/*shape change at the base mesh */
	CvMat*		__inv_pq;		/*inverse parameters at the base mesh*/
	
	CvMat*		__warp_t;		/*warp image to base mesh*/
	CvMat*		__error_t;		/*error between warp image and template image A0*/
	CvMat*		__search_pq;	/*search parameters */
	CvMat*		__delta_pq;		/*parameters change to be updated*/
	CvMat*		__current_s;		/*current search shape*/
	CvMat*		__update_s;		/*shape after composing the warp*/
	CvMat*		__delta_s;		/*shape change between two successive iteration*/
	CvMat*		__lamda;		/*appearance parameters*/
};
