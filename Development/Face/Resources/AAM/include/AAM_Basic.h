#pragma once

#include "AAM_TDM.h"
#include "AAM_CAM.h"

//! The basic Active appearace model building and fitting method
/*!
	\ingroup AAM

	Refer to: T.F. Cootes, G.J. Edwards and C.J. Taylor. "Active Appearance Models". ECCV 1998
*/
extern "C++" class __declspec(dllexport) AAM_Basic : public AAM  
{
public:
	//! Constructor
	AAM_Basic(const std::string& type);

	//! Destructor
	~AAM_Basic(void);

	//! Fit the image using aam basic. 
	virtual void Fit(const IplImage* image, AAM_Shape& Shape, int max_iter = 30, bool showprocess = false);

	//! Draw the image according the searching result(0:point, 1:triangle, 2:appearance)
	virtual void Draw(IplImage* image, int type = 0);

	//! Read data from stream 
	virtual void Read(std::ifstream& is, std::string& pModelFileName);

	//! Init search parameters 
	void InitParams(const IplImage* image);

	//! Get Mean Shape of model
	virtual inline const AAM_Shape GetMeanShape(void) const { 
		return __cam.__shape.GetAAMReferenceShape();	
	}

private:
	//! Calculates the pixel difference from a model instance and an image
	double EstResidual(const IplImage* image, const CvMat* c, CvMat* s, CvMat* q, CvMat* diff);
	
	//! Draw image with points
	void DrawPoint(IplImage* image, bool drawNumbers = false);

	//! Draw image with triangle mesh
	void DrawTriangle(IplImage* image);

	//! Draw image with full appearance
	void DrawAppearance(IplImage* image);

	//! Is the current shape within the image boundary?
	static void CheckShape(const CvMat* s, int w, int h);

	AAM_CAM __cam;
	CvMat*	__Rc;
	CvMat*	__Rq;
	
	//speed up for on-line alignment
	CvMat*	__current_c;	//current appearance parameters
	CvMat*  __update_c;		//update appearance parameters after certain iteration
	CvMat*	__delta_c;		//difference between successive c 
	CvMat*	__p;			//current shape parameters
	CvMat*	__current_q;	//current pose parameters
	CvMat*	__update_q;		//update pose parameters after certain iteration
	CvMat*	__delta_q;		//difference between two successive q
	CvMat*	__current_lamda;//current pose parameters
	CvMat*	__current_s;	//current shape
	CvMat*	__t_m;			//model texture instance
	CvMat*	__t_s;			//warped texture at current shape instance
	CvMat*	__delta_t;		//difference between __ts and __tm
};
