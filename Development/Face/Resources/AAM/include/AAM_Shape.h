#pragma once

#include "AAM_Util.h"

//! AAM Shape model.
/*!
	\ingroup AAM
*/
extern "C++" class __declspec(dllexport) AAM_Shape
{
public:
    //! Constructor
    AAM_Shape(void);

	//! Constructor
    AAM_Shape(const AAM_Shape &v);

	//! Destructor
    ~AAM_Shape(void);

	//! Access element
	const CvPoint2D32f operator[](int i) const;

	//! Access element
	CvPoint2D32f& operator[](int i);
	
    //! Operator = 
    AAM_Shape& operator=(const AAM_Shape &s);

	//! Operator +
    AAM_Shape operator+(const AAM_Shape &s);

	//! Operator -
    AAM_Shape operator-(const AAM_Shape &s);

	bool equalTo(const AAM_Shape &s);

	//! Minx of the shape
	const double MinX(void) const;

	//! Miny of the shape
    const double MinY(void) const;

	//! Maxx of the shape
    const double MaxX(void) const;

	//! Maxy of the shape
    const double MaxY(void) const;
    
    //! Get the center of Shape
    void COG(double &x, double &y) const;

	//! Translate Shape using (x, y) 
    void Translate(double x, double y);
	
	//! Align the shapes to reference shape 
	/*													 [a -b Tx]
		Returns the similarity transform: T(a,b,tx,ty) = [b  a Ty]
														 [0  0  1]
	*/
	void AlignTransformation(const AAM_Shape &ref, double &a, double &b, double &tx, double &ty) const;
        
	//! Transform Shape using similarity transform T(a,b,tx,ty)
	void TransformPose(double a, double b, double tx, double ty);

	//! Conversion between CvMat and AAM_Shape
	void Mat2Point(const CvMat* res);

	//! Conversion between CvMat and AAM_Shape
	void Point2Mat(CvMat* res) const;

	//! Read data from filestream
	void Read(std::ifstream& is);

	//! Size of the shape model
	inline const int NPoints(void) const {
		return m_vPoint.size(); 
	}

	//! Getting the vector of shape model
	inline std::vector<CvPoint2D32f> getPoints(void) const { 
		return m_vPoint; 
	}

	//! Clear the shape points
	inline void clear(void) { 
		resize(0);
	}

	//! Resize the shape
	inline void resize(int length) { 
		m_vPoint.resize(length, cvPoint2D32f(0.0, 0.0)); 
	}

	//! Get the shape width
	inline const double GetWidth(void) const {  
		return MaxX() - MinX();    
	}

	//! Get the shape height
	inline const double GetHeight(void) const { 
		return MaxY() - MinY();	
	}

private:
	void CopyData(const AAM_Shape &s);
	void Transform(double c00, double c01, double c10, double c11);

	// point data.
	std::vector<CvPoint2D32f> m_vPoint; 
};
