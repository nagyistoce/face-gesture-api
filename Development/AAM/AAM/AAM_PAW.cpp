#include <set>

#include <opencv2/legacy/legacy.hpp>
#include "AAM_PAW.h"

using namespace std;

//#define BINLINEAR 1

#define free2dvector(vec)										\
{																\
	for(int i = 0; i < vec.size(); i++) vec[i].clear();			\
	vec.clear();												\
}

AAM_PAW::AAM_PAW(void)
{
	__nTriangles = 0;
	__nPixels = 0;
	__n = 0;
	__width = 0;
	__height = 0;
}

AAM_PAW::~AAM_PAW(void)
{
	__pixTri.clear();
	__alpha.clear();
	__beta.clear();
	__gamma.clear();

	free2dvector(__rect);
	free2dvector(__vtri);
	free2dvector(__tri);
}

//============================================================================
void AAM_PAW::Train(const AAM_Shape& ReferenceShape, 
					CvMat* Points,
					CvMemStorage* Storage,
					const std::vector<std::vector<int> >* tri,
					bool buildVtri)
{
	__referenceshape = ReferenceShape;
	__n = __referenceshape.NPoints();// get the number of vertex point

	for(int i = 0; i < __n; i++)
		CV_MAT_ELEM(*Points, CvPoint2D32f, 0, i) = __referenceshape[i];

	CvMat* ConvexHull = cvCreateMat (1, __n, CV_64FC2);
	cvConvexHull2(Points, ConvexHull, CV_CLOCKWISE, 0);

	CvRect rect = cvBoundingRect(ConvexHull, 0);
	CvSubdiv2D* Subdiv = cvCreateSubdivDelaunay2D(rect, Storage);
	for(int ii = 0; ii < __n; ii++)
		cvSubdivDelaunay2DInsert(Subdiv, __referenceshape[ii]);

	//firstly: build triangle
	if(tri == 0)	Delaunay(Subdiv, ConvexHull);
	else	 __tri = *tri;
	__nTriangles = __tri.size();// get the number of triangles

	//secondly: build correspondence of Vertex-Triangle
	if(buildVtri)	FindVTri();

	//Thirdly: build pixel point in all triangles
	if(tri == 0) CalcPixelPoint(rect, ConvexHull);
	else FastCalcPixelPoint(rect);
	__nPixels = __pixTri.size();// get the number of pixels

	cvReleaseMat(&ConvexHull);
}


//============================================================================
void AAM_PAW::Delaunay(const CvSubdiv2D* Subdiv, const CvMat* ConvexHull)
{
	// firstly we build edges
	int i;
	CvSeqReader  reader;
	CvQuadEdge2D* edge;
	CvPoint2D32f org, dst;
	CvSubdiv2DPoint* org_pt, * dst_pt;
	std::vector<std::vector<int> > edges;
	std::vector<int> one_edge;     one_edge.resize(2);
	std::vector<int> one_tri;	one_tri.resize(3);
	int ind1, ind2;			

	cvStartReadSeq( (CvSeq*)(Subdiv->edges), &reader, 0 );
	for(i = 0; i < Subdiv->edges->total; i++)
	{
		edge = (CvQuadEdge2D*)(reader.ptr);
		if( CV_IS_SET_ELEM(edge)){
			org_pt = cvSubdiv2DEdgeOrg((CvSubdiv2DEdge)edge);
			dst_pt = cvSubdiv2DEdgeDst((CvSubdiv2DEdge)edge);

			if( org_pt && dst_pt ){
				org = org_pt->pt;
				dst = dst_pt->pt;
				if (cvPointPolygonTest(ConvexHull, org, 0) >= 0 &&
					cvPointPolygonTest( ConvexHull, dst, 0) >= 0){
						for (int j = 0; j < __n; j++){
							if (fabs(org.x-__referenceshape[j].x)<1e-6 &&
								fabs(org.y-__referenceshape[j].y)<1e-6)
							{
								for (int k = 0; k < __n; k++)
								{
									if (fabs(dst.x-__referenceshape[k].x)<1e-6
										&&fabs(dst.y-__referenceshape[k].y)<1e-6)
									{
										one_edge[0] = j;
										one_edge[1] = k;
										edges.push_back (one_edge);
									}
								}
							}
						}
				}
			}
		}

		CV_NEXT_SEQ_ELEM( Subdiv->edges->elem_size, reader );
	}

	// secondly we start to build triangles
	for (i = 0; i < edges.size(); i++)
	{
		ind1 = edges[i][0];
		ind2 = edges[i][1];

		for (int j = 0; j < __n; j++)
		{
			// At most, there are only 2 triangles that can be added 
			if(AAM_PAW::IsCurrentEdgeAlreadyIn(ind1, j, edges) && AAM_PAW::IsCurrentEdgeAlreadyIn(ind2, j, edges) )
			{
				one_tri[0] = ind1;
				one_tri[1] = ind2;
				one_tri[2] = j;
				if (AAM_PAW::VectorIsNotInFormerTriangles(one_tri, __tri) )
				{
					__tri.push_back(one_tri);
				}
			}
		}
	}

	//OK, up to now, we have already builded the triangles!
}


//============================================================================
bool AAM_PAW::IsCurrentEdgeAlreadyIn(int ind1, int ind2, const vector<vector<int> > &edges)
{
	for (int i = 0; i < edges.size (); i++)
	{
		if ((edges[i][0] == ind1 && edges[i][1] == ind2) || 
			(edges[i][0] == ind2 && edges[i][1] == ind1) )
			return true;
	}

	return false;
}

//============================================================================
bool AAM_PAW::VectorIsNotInFormerTriangles(const vector<int>& one_tri, const vector<vector<int> > &tris)
{
	set<int> tTriangle;
	set<int> sTriangle;

	for (int i = 0; i < tris.size (); i ++)
	{
		tTriangle.clear();
		sTriangle.clear();
		for (int j = 0; j < 3; j++ )
		{
			tTriangle.insert(tris[i][j]);
			sTriangle.insert(one_tri[j]);
		}
		if (tTriangle == sTriangle)    return false;
	}

	return true;
}

//============================================================================
void AAM_PAW::CalcPixelPoint(const CvRect rect, CvMat* ConvexHull)
{
	CvPoint2D32f point[3];
	CvMat tempVert = cvMat(1, 3, CV_64FC2, point);
	int ll = 0;
	double alpha, belta, gamma;
	CvPoint2D32f pt;
	int ind1, ind2, ind3;
	int ii, jj;
	double x, y, x1, y1, x2, y2, x3, y3, c;

	__xmin = rect.x;
	__ymin = rect.y;
	__width = rect.width;
	__height = rect.height;
	int left = rect.x, right = left + __width;
	int top = rect.y, bottom = top + __height;

	__rect.resize(__height);
	for (int i = top; i < bottom; i++)
	{
		ii = i - top;
		__rect[ii].resize(__width);
		for (int j = left; j < right; j++)
		{
			jj = j - left;
			pt = cvPoint2D32f(j, i);
			__rect[ii][jj] = -1;

			// firstly: the point (j, i) is located in the ConvexHull
			if(cvPointPolygonTest(ConvexHull, pt, 0) >= 0 )
			{
				// then we find the triangle that the point lies in
				for (int k = 0; k < __nTriangles; k++)
				{
					ind1 = __tri[k][0];
					ind2 = __tri[k][1];
					ind3 = __tri[k][2];
					point[0] = __referenceshape[ind1];
					point[1] = __referenceshape[ind2];
					point[2] = __referenceshape[ind3];

					// secondly: the point(j,i) is located in the k-th triangle
					if(cvPointPolygonTest(&tempVert, pt, 0) >= 0)
					{
						__rect[ii][jj] = ll++;
						__pixTri.push_back(k);

						// calculate alpha and belta for warp
						x = j;		 y = i;
						x1 = point[0].x; y1 = point[0].y;
						x2 = point[1].x; y2 = point[1].y;
						x3 = point[2].x; y3 = point[2].y,

							c = 1.0/(+x2*y3-x2*y1-x1*y3-x3*y2+x3*y1+x1*y2);
						alpha = (y*x3-y3*x+x*y2-x2*y+x2*y3-x3*y2)*c;
						belta = (-y*x3+x1*y+x3*y1+y3*x-x1*y3-x*y1)*c;
						gamma = 1 - alpha - belta; 

						__alpha.push_back(alpha);
						__beta.push_back(belta);
						__gamma.push_back(gamma);

						// make sure each point only located in only one triangle
						break;
					}

				}
			}
		}
	}
}


//============================================================================
void AAM_PAW::FastCalcPixelPoint(const CvRect rect)
{
	CvPoint2D32f point[3];
	CvMat oneTri = cvMat(1, 3, CV_64FC2, point);
	double alpha, belta, gamma;
	CvPoint2D32f pt;
	int ind1, ind2, ind3;
	int ll = 0;
	double x, y, x1, y1, x2, y2, x3, y3, c;

	__xmin = rect.x;			__ymin = rect.y;
	__width = rect.width;		__height = rect.height;
	int left = rect.x, top = rect.y;
	double aa, bb, cc, dd;

	__rect.resize(__height);
	for(int i = 0; i < __height; i++) 
	{
		__rect[i].resize(__width);
		for(int j = 0; j < __width; j++)
			__rect[i][j] = -1;
	}

	for(int k = 0; k < __nTriangles; k++)
	{
		ind1 = __tri[k][0];
		ind2 = __tri[k][1];
		ind3 = __tri[k][2];

		point[0] = __referenceshape[ind1];
		point[1] = __referenceshape[ind2];
		point[2] = __referenceshape[ind3];

		x1 = point[0].x; y1 = point[0].y;
		x2 = point[1].x; y2 = point[1].y;
		x3 = point[2].x; y3 = point[2].y;
		c = 1.0/(+x2*y3-x2*y1-x1*y3-x3*y2+x3*y1+x1*y2);

		aa = MIN(x1, MIN(x2, x3)); //left x
		bb = MAX(x1, MAX(x2, x3)); //right x
		cc = MIN(y1, MIN(y2, y3)); //top y
		dd = MAX(y1, MAX(y2, y3)); //bot y

		for(y=ceil(cc); y<=dd;y +=1)
		{
			for(x=ceil(aa); x<=bb;x+=1)
			{
				pt = cvPoint2D32f(x, y);
				//the point is located in the k-th triangle
				if(cvPointPolygonTest(&oneTri, pt, 0) >= 0)
				{
					__rect[y-top][x-left] = ll++;
					__pixTri.push_back(k);

					alpha = (y*x3-y3*x+x*y2-x2*y+x2*y3-x3*y2)*c;
					belta = (-y*x3+x1*y+x3*y1+y3*x-x1*y3-x*y1)*c;
					gamma = 1 - alpha - belta; 

					__alpha.push_back(alpha);
					__beta.push_back(belta);
					__gamma.push_back(gamma);
				}

			}
		}
	}
}


//============================================================================
void AAM_PAW::FindVTri()
{
	__vtri.resize(__n);
	for(int i = 0; i < __n; i++)
	{
		for(int j = 0; j < __nTriangles; j++)
		{
			if(__tri[j][0] == i || __tri[j][1] == i || __tri[j][2] == i)
				__vtri[i].push_back(j);
		}
	}
}

//============================================================================
void AAM_PAW::CalcWarpParameters(double x, double y, double x1, double y1,
		double x2, double y2, double x3, double y3, 
		double &alpha, double &beta, double &gamma)
{
	double c = (+x2*y3-x2*y1-x1*y3-x3*y2+x3*y1+x1*y2);
	alpha = (y*x3-y3*x+x*y2-x2*y+x2*y3-x3*y2) / c;
	beta  = (-y*x3+x1*y+x3*y1+y3*x-x1*y3-x*y1) / c;
	gamma = 1 - alpha - beta; 
}

//============================================================================
void AAM_PAW::Warp(double x, double y, 
				   double x1, double y1, double x2, double y2, double x3, double y3, 
				   double& X, double& Y, 
				   double X1, double Y1, double X2, double Y2, double X3, double Y3)
{
	double c = 1.0/(+x2*y3-x2*y1-x1*y3-x3*y2+x3*y1+x1*y2);
	double alpha = (y*x3-y3*x+x*y2-x2*y+x2*y3-x3*y2)*c;
	double belta  = (-y*x3+x1*y+x3*y1+y3*x-x1*y3-x*y1)*c;
	double gamma = 1.0 - alpha - belta; 

	X = alpha*X1 + belta*X2 + gamma*X3;
	Y = alpha*Y1 + belta*Y2 + gamma*Y3;
}

//============================================================================
void AAM_PAW::CalcWarpTexture(const CvMat* s, const IplImage* image, CvMat* t) const
{
	double *fastt = t->data.db;
	double *ss = s->data.db;
	int v1, v2, v3, tri_idx;
	double x, y;
	int X, Y;
	int ixB1, ixG1, ixR1;
	unsigned char* p1;

#ifdef BINLINEAR
	int X1, Y1;
	double s0 , t0, s1, t1;
	int ixB2, ixG2, ixR2;
	unsigned char* p2;
#endif

	char* imgdata = image->imageData;
	int step = image->widthStep;
	int nchannel = image->nChannels;
	int off_g = (nchannel == 3) ? 1 : 0;
	int off_r = (nchannel == 3) ? 2 : 0;
	
	for(int i = 0, k = 0; i < __nPixels; i++, k+=3)
	{
		tri_idx = __pixTri[i];
		v1 = __tri[tri_idx][0];
		v2 = __tri[tri_idx][1];
		v3 = __tri[tri_idx][2];

		x = __alpha[i]*ss[v1<<1] + __beta[i]*ss[v2<<1] + 
			__gamma[i]*ss[v3<<1];
		y = __alpha[i]*ss[1+(v1<<1)] + __beta[i]*ss[1+(v2<<1)] + 
			__gamma[i]*ss[1+(v3<<1)];

#ifdef BINLINEAR
		X = cvFloor(x);	Y = cvFloor(y);	X1 = cvCeil(x);	Y1 = cvCeil(y);
		s0 = x-X;		t0 = y-Y;		s1 = 1-s0;		t1 = 1-t0;

		ixB1 = nchannel*X; ixG1= ixB1+off_g;	ixR1 = ixB1+off_r;	
		ixB2 = nchannel*X1;	ixG2= ixB2+off_g;	ixR2 = ixB2+off_r;	
		
		p1 = (unsigned char*)(imgdata + step*Y);
		p2 = (unsigned char*)(imgdata + step*Y1);

		fastt[k	] = s1*(t1*p1[ixB1]+t0*p2[ixB1])+s0*(t1*p1[ixB2]+t0*p2[ixB2]);
		fastt[k+1] = s1*(t1*p1[ixG1]+t0*p2[ixG1])+s0*(t1*p1[ixG2]+t0*p2[ixG2]);
		fastt[k+2] = s1*(t1*p1[ixR1]+t0*p2[ixR1])+s0*(t1*p1[ixR2]+t0*p2[ixR2]);

#else
		X = cvFloor(x);		Y = cvFloor(y);
		ixB1 = X*nchannel;	ixG1= ixB1+off_g;	ixR1 = ixB1+off_r;
		p1 = (unsigned char*)(imgdata + step*Y);
		fastt[k	] = p1[ixB1];
		fastt[k+1] = p1[ixG1];
		fastt[k+2] = p1[ixR1];
#endif

	}
}

void AAM_PAW::Read(std::ifstream& is)
{
	int i, j;
	is >> __n >> __nTriangles >> __nPixels >> __xmin >> __ymin >> __width >> __height;
	
	__tri.resize(__nTriangles);
	for(i = 0; i < __nTriangles; i++)
	{
		__tri[i].resize(3);
		is >> __tri[i][0] >> __tri[i][1] >> __tri[i][2];
	}

	__vtri.resize(__n);
	for(i = 0; i < __n; i++)
	{
		__vtri[i].resize(__nTriangles);
		for( j = 0; j < __nTriangles; j++)	is >> __vtri[i][j];
	}

	__pixTri.resize(__nPixels);
	for(i = 0;  i < __nPixels; i++)	is >> __pixTri[i];
	
	__alpha.resize(__nPixels);
	for(i = 0; i < __nPixels; i++)	is >> __alpha[i];

	__beta.resize(__nPixels);
	for(i = 0; i < __nPixels; i++)	is >> __beta[i];
	
	__gamma.resize(__nPixels);
	for(i = 0; i < __nPixels; i++)	is >> __gamma[i];
	
	__rect.resize(__height);
	for(i = 0; i < __height; i++)
	{
		__rect[i].resize(__width);
		for(j = 0; j < __width; j++) is >> __rect[i][j];
	}
	
	__referenceshape.resize(__n);
	__referenceshape.Read(is);
}
