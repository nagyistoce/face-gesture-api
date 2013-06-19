#pragma once
#pragma warning( disable: 4251 4018 4996 4224 4244 )

/*!
	\defgroup AAM Active Appearance Models library.
*/

#include <string>
#include <fstream>

#include "opencv2/opencv.hpp"

std::ostream& operator<<(std::ostream &os, const CvMat* mat);
std::istream& operator>>(std::istream &is, CvMat* mat);

class AAM_Shape;

//! Main AAM class.
/*!
	\ingroup AAM
*/
extern "C++" class __declspec(dllexport) AAM
{
public:
	//! Constructor
	AAM(void);

	//! Destructor
	virtual ~AAM(void) = NULL;

	//! Fit the image using AAM 
	virtual void Fit(const IplImage* image, AAM_Shape& Shape, int max_iter = 30, bool showprocess = false) = NULL;

	//! Draw the image according search result
	virtual void Draw(IplImage* image, int type = 0) = NULL;

	//! Read data from stream 
	virtual void Read(std::ifstream& is, std::string& pModelFileName) = NULL;

	//! Get Mean Shape of model
	virtual const AAM_Shape GetMeanShape(void) const = NULL;

protected:
	//void AAM::SmoothImagePoints(AAM_Shape& Shape);

	//std::string type_;
	//AAM_Shape* prevShape_;

private:
	//double w1_;
	//double w2_;
};
