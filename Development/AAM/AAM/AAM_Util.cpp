#include "AAM_Util.h"
#include "AAM_Shape.h"

using namespace std;

ostream& operator<<(ostream &os, const CvMat* mat)
{
	for(int i = 0; i < mat->rows; i++)
	{
		for(int j = 0; j < mat->cols; j++)
			os << CV_MAT_ELEM(*mat, double, i, j) << " ";

		os << endl;
	}
	return os;
}

istream& operator>>(istream &is, CvMat* mat)
{
	for(int i = 0; i < mat->rows; i++)
		for(int j = 0; j < mat->cols; j++)
			is >> CV_MAT_ELEM(*mat, double, i, j);

	return is;
}

AAM::AAM(void)
//:	type_(type),
	//prevShape_(NULL),
	//w1_(0.5),
	//w2_(0.5)
{
}

AAM::~AAM(void)
{
}

//void AAM::SmoothImagePoints(AAM_Shape& Shape)
//{
//	if(prevShape_->NPoints() != Shape.NPoints())
//		return;
//
//	AAM_Shape smoothedShape(Shape);
//
//	for(int i = 0; i < Shape.NPoints(); i++)
//	{
//		smoothedShape[i].x = w1_ * (*prevShape_)[i].x + w2_ * Shape[i].x;
//		(*prevShape_)[i].x = Shape[i].x;
//
//		smoothedShape[i].y = w1_ * (*prevShape_)[i].y + w2_ * Shape[i].y;
//		(*prevShape_)[i].y = Shape[i].y;
//	}
//
//	Shape = smoothedShape;
//}