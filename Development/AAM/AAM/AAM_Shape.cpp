#include "AAM_Shape.h"

using namespace std;

AAM_Shape::AAM_Shape(void)
{ 
	resize(0); 
}

AAM_Shape::AAM_Shape(const AAM_Shape &s)
{
    CopyData(s);
}

AAM_Shape::~AAM_Shape(void) 
{ 
	clear(); 
}

const CvPoint2D32f AAM_Shape::operator[](int i) const 
{
	return m_vPoint[i];
}

CvPoint2D32f& AAM_Shape::operator[](int i) 
{
	return m_vPoint[i];	
}

AAM_Shape& AAM_Shape::operator=(const AAM_Shape &s)
{
    CopyData(s);
    return *this;
}

AAM_Shape AAM_Shape::operator+(const AAM_Shape &s)
{    
	AAM_Shape res(*this);
	for (int i = 0, size = m_vPoint.size(); i < size; i++)
	{
		res.m_vPoint[i].x += s.m_vPoint[i].x;
		res.m_vPoint[i].y += s.m_vPoint[i].y;
	}
	return res;
	
}

AAM_Shape AAM_Shape::operator-(const AAM_Shape &s)
{    
	AAM_Shape res(*this);
	for (int i = 0, size = m_vPoint.size(); i < size; i++)
	{
		res.m_vPoint[i].x -= s.m_vPoint[i].x;
		res.m_vPoint[i].y -= s.m_vPoint[i].y;
	}
	return res;
}

bool AAM_Shape::equalTo(const AAM_Shape &s)
{
	int size = m_vPoint.size();

	if(s.NPoints() != size)
		return false;

	for(int i = 0; i < size; i++)
	{
		if( fabs(m_vPoint[i].x - s[i].x) > 1e-6)
			return false;

		if( fabs(m_vPoint[i].y - s[i].y) > 1e-6)
			return false;
	}

	return true;
}

void AAM_Shape::CopyData(const AAM_Shape &s )
{
	if(m_vPoint.size() != s.m_vPoint.size())
		m_vPoint.resize(s.m_vPoint.size());
	m_vPoint = s.m_vPoint;
}

//---------------------------------------------------------------------
// Get the center of Shape
//----------------------------------------------------------------------
void AAM_Shape::COG(double &x, double &y)const
{
	x = y = 0.0;

	for(int i = 0, size = m_vPoint.size(); i < size; i++)
	{
		x += m_vPoint[i].x;
		y += m_vPoint[i].y;
	}

	x /= m_vPoint.size();
	y /= m_vPoint.size();
}

//---------------------------------------------------------------------
// Translate Shape using (x, y) 
//----------------------------------------------------------------------
void AAM_Shape::Translate(double x, double y)
{
    for(int i = 0, size = m_vPoint.size(); i < size; i++)
    {
        m_vPoint[i].x += x;
        m_vPoint[i].y += y;
    }
}

//----------------------------------------------------------------------
// Shape = (c00, c01; c10, c11) * Shape
//----------------------------------------------------------------------
void AAM_Shape::Transform(double c00, double c01, double c10, double c11)
{
	double x, y;

	for(int i = 0, size = m_vPoint.size(); i < size; i++)
	{
		x = m_vPoint[i].x;
		y = m_vPoint[i].y;
		m_vPoint[i].x = c00*x+c01*y;
		m_vPoint[i].y = c10*x+c11*y;
	}
}

// ---------------------------------------------------------------------------
// Align Shape to to RefShape using a similarity transform.
// See also algorithm C.3 in Appendix C of [Cootes & Taylor, 2004]
// www.isbe.man.ac.uk/~bim/Models/app_models.pdf.
// returns the similarity transform matrix:
//      a -b  Tx
//      b  a  Ty
//      0  0  1
// Note BY YAO Wei: the Shape itself doesn't change
// ---------------------------------------------------------------------------
void AAM_Shape::AlignTransformation(const AAM_Shape &ref, double &a, double &b, double &tx, double &ty) const
{

	double X1 = 0, Y1 = 0, X2 = 0, Y2 = 0, Z = 0, C1 = 0, C2 = 0;
	double W = m_vPoint.size();
	double x1, y1, x2, y2;

	for(int i = 0, size = m_vPoint.size(); i < size; i++)
	{
		x1 = ref.m_vPoint[i].x;
		y1 = ref.m_vPoint[i].y;
		x2 = m_vPoint[i].x;
		y2 = m_vPoint[i].y;

		Z  += x2 * x2 + y2 * y2;
		X1 += x1;
		Y1 += y1;
		X2 += x2;
		Y2 += y2;
		C1 += x1 * x2 + y1 * y2;
		C2 += y1 * x2 - x1 * y2;
	}

	{
		double SolnA[] = {X2, -Y2, W, 0, Y2, X2, 0, W, Z, 0, X2, Y2, 0, Z, -Y2, X2};
		CvMat A = cvMat(4, 4, CV_64FC1, SolnA);
		double SolnB[] = {X1, Y1, C1, C2};
		CvMat B = cvMat(4, 1, CV_64FC1, SolnB);

		static CvMat* Soln = cvCreateMat(4, 1, CV_64FC1);
		cvSolve(&A, &B, Soln, CV_SVD);

		a	= CV_MAT_ELEM(*Soln, double, 0, 0);  b	= CV_MAT_ELEM(*Soln, double, 1, 0);
		tx	= CV_MAT_ELEM(*Soln, double, 2, 0);	 ty	= CV_MAT_ELEM(*Soln, double, 3, 0);
	}

	// Explained by YAO Wei, 2008.01.29.
	// It is equivalent as follows, but the former method of is more robust.
	/************************************************************************/
	/*		a	=	C1 / Z			b	=	C2 / Z							*/
	/*		tx	=	X1 / W			ty	=	Y1 / W							*/
	/************************************************************************/	
}

//----------------------------------------------------------------------
// Shape = (a, -b; b, a) * Shape + (tx, ty)
//----------------------------------------------------------------------
void AAM_Shape::TransformPose(double a, double b, double tx, double ty)
{
    Transform(a, -b, b, a);
	
	Translate( tx, ty );
}

const double AAM_Shape::MinX(void) const
{	
	double val, min = DBL_MAX;

	for(int i = 0, size = m_vPoint.size(); i < size; i++)
	{	
		val = m_vPoint[i].x;
		min = val<min ? val : min;
	}
	return min;
}

const double AAM_Shape::MinY(void) const
{	
	double val, min = DBL_MAX;
	for(int i = 0, size = m_vPoint.size(); i < size; i++)
	{	
		val = m_vPoint[i].y;
		min = val<min ? val : min;
	}
	return min;
}

const double AAM_Shape::MaxX(void) const
{	
	double val, max = -DBL_MAX;
	for(int i = 0, size = m_vPoint.size(); i < size; i++)
	{
		val = m_vPoint[i].x;
		max = val>max ? val : max;
	}
	return max;
}


const double AAM_Shape::MaxY(void) const
{	
	double val, max = -DBL_MAX;
	for(int i = 0, size = m_vPoint.size(); i < size; i++)
	{
		val = m_vPoint[i].y;
		max = val>max ? val : max;
	}
	return max;
}
	
void AAM_Shape::Mat2Point(const CvMat* res)
{
    int nPoints = res->cols / 2;
	double* B = (double*)(res->data.ptr + res->step * 0);

	if(m_vPoint.size() != nPoints)		resize(nPoints);

	//if( B[0] < -10000 || B[0] > 10000 )	
	//	return;

	for (int i = 0; i < nPoints ; i++)
    {
		m_vPoint[i].x = cvGetReal1D(res, 2*i);
		m_vPoint[i].y = cvGetReal1D(res, 2*i+1);
	}
}

void AAM_Shape::Point2Mat(CvMat* res) const
{
	int nPoints = res->cols / 2;

	for (int i = 0; i < nPoints ; i++)
    {
		cvSetReal1D(res, 2 * i, m_vPoint[i].x);
		cvSetReal1D(res, 2 * i + 1, m_vPoint[i].y);
	}
}

void AAM_Shape::Read(std::ifstream& is)
{
	for (int i = 0, nPoints = NPoints(); i < nPoints ; i++)
		is >> m_vPoint[i].x >> m_vPoint[i].y;
}
