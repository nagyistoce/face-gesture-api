#pragma once

#include "AAM_Shape.h"

class AAM_IC;
class AAM_Basic;

//! 2D piecewise affine warp
/*!
	\ingroup AAM
*/
extern "C++" class __declspec(dllexport) AAM_PAW  
{
	friend class AAM_IC;
	friend class AAM_Basic;
public:
	//! Constructor
	AAM_PAW(void);

	//! Destructor
	~AAM_PAW(void);

	//! Build a piecewise affine warp
	void Train(const AAM_Shape& ReferenceShape, CvMat* Points, CvMemStorage* Storage, 
		const std::vector<std::vector<int> >* tri = 0, bool buildVtri = true);

	//! Read data from stream 
	void Read(std::ifstream& is);

	//! Warp the texture over a shape
	void CalcWarpTexture(const CvMat* Shape, const IplImage* image, CvMat* Texture) const;

	//! Get the warp parameters for a specific point according the piecewise
	static void CalcWarpParameters(double x, double y, double x1, double y1, 
		double x2, double y2, double x3, double y3, 
		double &alpha, double &beta, double &gamma);

	//! Warp from source triangle to destination triangle
	static void Warp(double x, double y, double x1, double y1,
		double x2, double y2, double x3, double y3,
		double& X, double& Y, double X1, double Y1,
		double X2, double Y2, double X3, double Y3);

	//! Number of points
	inline const int nPoints() const { 
		return __n;	
	}

	//! Number of pixels
	inline const int nPix() const { 
		return __nPixels;	
	}

	//! Number of triangles
	inline const int nTri() const { 
		return __nTriangles;	
	}

	//! Get triangle
	inline const std::vector<std::vector<int> >* GetTri() const {  
		return &__tri;		
	}

	//! Index of point for j-th vertex of i-th triangle 
	inline const int Tri(int i, int j) const { 
		return __tri[i][j];	
	}

	//! Does vertex @i share @j-th triangle
	inline const int vTri(int i, int j) const { 
		return __vtri[i][j];	
	}

	//! Index of triangle the pixel lies in
	inline const int PixTri(int i)const{ return __pixTri[i];	}

	//! Coeffiects of affine warp
	inline const double Alpha(int i) const { 
		return __alpha[i];	
	}

	//! Coeffiects of affine warp
	inline const double Beta(int i) const { 
		return __beta[i];	
	}

	//! Coeffiects of affine warp
	inline const double Gamma(int i) const { 
		return __gamma[i];	
	}

	//! Is point(j,i) in boundary: not(-1), yes(index of pixel)
	inline const int Rect(int i, int j) const { 
		return __rect[i][j]; 
	} 

private:
	//! build triangles
	void Delaunay(const CvSubdiv2D* Subdiv, const CvMat *ConvexHull);

	//! calculate all pixels in the triangles
	void CalcPixelPoint(const CvRect rect, CvMat *ConvexHull);

	//! calculate all pixels in the triangles
	void FastCalcPixelPoint(const CvRect rect);

	//! Is the current edge (ind1, ind2) already in the AAM model edges?
	static bool IsCurrentEdgeAlreadyIn(int ind1, int ind2, const std::vector<std::vector<int> > &edges);

	//! Help to build up triangles in the mesh
	static bool VectorIsNotInFormerTriangles(const std::vector<int>& one_tri, const std::vector<std::vector<int> > &tris);

	//! Find triangles containing each landmark.
	void FindVTri(void);

	int __n;								/*number of landmarks*/
	int __nPixels;							/*number of pixels*/
	int __nTriangles;						/*number of triangles*/
	int __width, __height, __xmin, __ymin;	/*Domain of warp region*/
	
	std::vector<std::vector<int> > __tri;	/*triangle vertexes index*/
	std::vector<std::vector<int> > __vtri;	/*vertex vs triangle*/
	std::vector<int>			   __pixTri;
	std::vector<double>			   __alpha, __beta,  __gamma; 
	std::vector<std::vector<int> > __rect;	/*height by width */

	AAM_Shape __referenceshape; 
};
