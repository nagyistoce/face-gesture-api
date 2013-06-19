#include "AAM_IC.h"

#define MAG(a, b) sqrt((a) * (a) + (b) * (b))
#define curtime cvGetTickCount() / (cvGetTickFrequency() * 1000.0)

using namespace std;

typedef unsigned char byte;

AAM_IC::AAM_IC(void)
:	AAM()
{
	__Points = 0;
	__Storage = 0;

	__update_s0 = 0;
	__delta_s = 0;
	__warp_t = 0;
	__error_t = 0;
	__search_pq = 0;
	__delta_pq = 0;
	__current_s = 0;
	__update_s = 0;
}

AAM_IC::~AAM_IC(void)
{
	cvReleaseMat(&__Points);
	cvReleaseMemStorage(&__Storage);

	cvReleaseMat(&__update_s0);
	cvReleaseMat(&__delta_s);
	cvReleaseMat(&__warp_t);
	cvReleaseMat(&__error_t);
	cvReleaseMat(&__search_pq);
	cvReleaseMat(&__delta_pq);
	cvReleaseMat(&__current_s);
	cvReleaseMat(&__update_s);

	//delete prevShape_;
}

//============================================================================
void AAM_IC::Fit(const IplImage* image, AAM_Shape& Shape, int max_iter, bool showprocess)
{
	//initialize some stuff
	double t = curtime;
	double e1(1e100), e2;
	const CvMat* A0 = __texture.GetMean();
	CvMat p; cvGetCols(__search_pq, &p, 4, 4+__shape.nModes());
	Shape.Point2Mat(__current_s);
	SetAllParamsZero();
	__shape.CalcParams(__current_s, __search_pq);

	int iter;

	for( iter = 0; iter < max_iter; iter++)
	{
		if(showprocess)
		{	
			IplImage* Drawimg = cvCloneImage(image);
			char filename[100];

			Shape.Mat2Point(__current_s);
			Draw(Drawimg, 2);
			sprintf(filename, "result/Iter-%02d.jpg", iter);

			cvSaveImage(filename, Drawimg);
			cvReleaseImage(&Drawimg);
		}

		//check the current shape
		CheckShape(__current_s, image->width, image->height);

		//warp image to mesh shape mesh
		__paw.CalcWarpTexture(__current_s, image, __warp_t);
		AAM_TDM::AlignTextureToRef(A0, __warp_t);
		cvSub(__warp_t, A0, __error_t);

		//check for texture divergence
		e2 = cvNorm(__error_t);
		if(e2 < 0.01 || (float(iter) > max_iter / 3.0 && fabs(e2 - e1) < 0.01 * e1)) 
			break;
		e1 = e2;

		//calculate updates (and scale to account for linear lighting gain)
		cvGEMM(__error_t, __G, 1, NULL, 0, __delta_pq, CV_GEMM_B_T);

		//check for parameter convergence
		if(cvNorm(__delta_pq) < 1e-6)	break;

		//apply inverse compositional algorithm to update parameters
		InverseCompose(__delta_pq, __current_s, __update_s);

		//smooth shape
		cvAddWeighted(__current_s, 0.4, __update_s, 0.6, 0, __update_s);

		//update parameters
		__shape.CalcParams(__update_s, __search_pq);

		//calculate constrained new shape
		__shape.CalcShape(__search_pq, __update_s);

		//check for shape convergence
		if(cvNorm(__current_s, __update_s, CV_L2) < 0.001)	
			break;
		else 
			cvCopy(__update_s, __current_s);
	}

	//if(prevShape_ == NULL)
	//	prevShape_ = new AAM_Shape(Shape);
	//else if(prevShape_->equalTo(Shape) == false)
	//	SmoothImagePoints(Shape);

	Shape.Point2Mat(__current_s);
	t = curtime - t;

	cout << "AAM-IC Fitting time cost: " << t << " millisec." << endl;
	cout << "Number of iterations: " << iter << "." << endl;
}

void AAM_IC::SetAllParamsZero()
{
	cvZero(__warp_t);
	cvZero(__error_t);
	cvZero(__search_pq);
	cvZero(__delta_pq);
	cvZero(__lamda);
}

//============================================================================
void AAM_IC::InverseCompose(const CvMat* dpq, const CvMat* s, CvMat* NewS)
{
	// Firstly: Estimate the corresponding changes to the base mesh
	cvConvertScale(dpq, __inv_pq, -1);
	__shape.CalcShape(__inv_pq, __update_s0);	// __update_s0 = N.W(s0, -delta_p, -delta_q)

	//Secondly: Composing the Incremental Warp with the Current Warp Estimate.
	double *S0 = __update_s0->data.db;
	double *S = s->data.db;
	double *SEst = NewS->data.db;

	for(int i = 0; i < __shape.nPoints(); i++)
	{
		double x = 0.0;
		double y = 0.0;
		int k = 0;

		//The only problem with this approach is which triangle do we use?
		//In general there will be several triangles that share the i-th vertex.
		for(int j = 0; j < __paw.nTri(); j++)
		{
			if(__paw.vTri(i, j) > 0)
			{
				double xw, yw;
				int v1 = __paw.Tri(j, 0);
				int v2 = __paw.Tri(j, 1);
				int v3 = __paw.Tri(j, 2);

				AAM_PAW::Warp(
					S0[2*i], S0[2*i+1],
					__sMean[v1].x, __sMean[v1].y, __sMean[v2].x, __sMean[v2].y, __sMean[v3].x, __sMean[v3].y,
					xw, yw,	
					S[2*v1], S[2*v1+1], S[2*v2], S[2*v2+1], S[2*v3], S[2*v3+1]
				);
			
				x += xw, y += yw, k++;
			}
		}

		// average the result so as to smooth the warp at each vertex
		SEst[2*i] = x/k, SEst[2*i+1] = y/k;
	}
}

//============================================================================
void AAM_IC::Draw(IplImage* image, int type)
{
	if(type == 0) 
		DrawPoint(image);
	else if(type == 1) 
		DrawTriangle(image);
	else if(type == 2) 
	{
		cvGEMM(__error_t, __texture.GetBases(), 1, NULL, 0, __lamda, CV_GEMM_B_T);
		__texture.CalcTexture(__lamda, __warp_t);
		DrawAppearance(image);
	}
	else 
		CV_Error(-1, "Unsupported AAM drawing type!");
}

//============================================================================
void AAM_IC::DrawPoint(IplImage* image, bool drawNumbers)
{
	double* p = __current_s->data.db;

	for(int i = 0; i < __shape.nPoints(); i++)
	{
		cvCircle(image, cvPoint(p[2*i], p[2*i+1]), 1, CV_RGB(0, 0, 255), -1);

		if( drawNumbers )
		{
			CvFont font;
			char text[1000];
			sprintf(text, "%d", i);

			cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 0.2, 0.2, 0, 2);
			cvPutText(image, text, cvPoint(p[2*i], p[2*i+1]), &font, cvScalar(255, 255, 255, 0));
		}
	}
}

//============================================================================
void AAM_IC::DrawTriangle(IplImage* image)
{
	double* p = __current_s->data.db;
	int idx1, idx2, idx3;
	for(int i = 0; i < __paw.nTri(); i++)
	{
		idx1 = __paw.__tri[i][0]; 
		idx2 = __paw.__tri[i][1]; 
		idx3 = __paw.__tri[i][2];

		cvLine(image, cvPoint(p[2*idx1], p[2*idx1+1]), cvPoint(p[2*idx2], p[2*idx2+1]),	CV_RGB(128,255,0));
		cvLine(image, cvPoint(p[2*idx2], p[2*idx2+1]), cvPoint(p[2*idx3], p[2*idx3+1]),	CV_RGB(128,255,0));
		cvLine(image, cvPoint(p[2*idx3], p[2*idx3+1]), cvPoint(p[2*idx1], p[2*idx1+1]),	CV_RGB(128,255,0));
	}
}

//============================================================================
void AAM_IC::DrawAppearance(IplImage* image)
{
	AAM_Shape Shape; Shape.Mat2Point(__current_s);
	AAM_PAW paw;
	paw.Train(Shape, __Points, __Storage, __paw.GetTri(), false);
	int x1, x2, y1, y2, idx1, idx2;
	int xby3, idxby3;
	int minx, miny, maxx, maxy;
	int tri_idx, v1, v2, v3;
	AAM_Shape refShape = __sMean;
	refShape.Translate(-refShape.MinX(), -refShape.MinY());
	double minV, maxV;
	cvMinMaxLoc(__warp_t, &minV, &maxV);
	cvConvertScale(__warp_t, __warp_t, 255/(maxV-minV), -minV*255/(maxV-minV));
	byte* pimg;
	double* fastt = __warp_t->data.db;

	minx = Shape.MinX(); miny = Shape.MinY();
	maxx = Shape.MaxX(); maxy = Shape.MaxY();

	if( minx < 0 )
		minx = 0;
	else if(minx >= image->width)
		minx = image->width - 1;

	if( miny < 0 )
		miny = 0;
	else if(miny >= image->height)
		miny = image->height - 1;

	if( maxx < 0 )
		maxx = 0;
	else if(maxx >= image->width)
		maxx = image->height - 1;

	if( maxy < 0 )
		maxy = 0;
	else if(maxy >= image->height)
		maxy = image->height - 1;

	for(int y = miny; y < maxy; y++)
	{
		y1 = y-miny;
		pimg = (byte*)(image->imageData + image->widthStep*y);
		for(int x = minx; x < maxx; x++)
		{
			x1 = x-minx;
			idx1 = paw.__rect[y1][x1];
			if(idx1 >= 0)
			{
				tri_idx = paw.__pixTri[idx1];
				v1 = paw.__tri[tri_idx][0];
				v2 = paw.__tri[tri_idx][1];
				v3 = paw.__tri[tri_idx][2];
		
				x2 = paw.__alpha[idx1]*refShape[v1].x + paw.__beta[idx1]*refShape[v2].x +  
					paw.__gamma[idx1]*refShape[v3].x;
				y2 = paw.__alpha[idx1]*refShape[v1].y + paw.__beta[idx1]*refShape[v2].y +  
					paw.__gamma[idx1]*refShape[v3].y;
				
				xby3 = 3*x; 
				idx2 = __paw.__rect[y2][x2];		idxby3 = 3*idx2;
				pimg[xby3] = fastt[idxby3];
				pimg[xby3+1] = fastt[idxby3+1];
				pimg[xby3+2] = fastt[idxby3+2];
			}
		}
	}
}


//============================================================================
// TODO: Így kicsit lassabb. Visszarakni boolra.
void AAM_IC::CheckShape(const CvMat* s, int w, int h)
{
	double* fasts = s->data.db;
	int npoints = s->cols / 2;

	for(int i = 0; i < npoints; i++)
	{
		if(fasts[2*i] > w-1) 
			fasts[2*i] = w-1;
		else if(fasts[2*i] < 0) 
			fasts[2*i] = 0;

		if(fasts[2*i+1] > h-1) 
			fasts[2*i+1] = h-1;
		else if(fasts[2*i+1] < 0) 
			fasts[2*i+1] = 0;
	}
}

//============================================================================
void AAM_IC::Read(std::ifstream& is, std::string& pModelFileName)
{
	cout << "Reading the AAM-IC Model from file " << pModelFileName << "..." << endl;

	__shape.Read(is);
	__texture.Read(is);
	__paw.Read(is);

	int nPoints = __shape.nPoints();
	__sMean.resize(nPoints);
	__sStar1.resize(nPoints); __sStar2.resize(nPoints);
	__sStar3.resize(nPoints); __sStar4.resize(nPoints);
	__sMean.Read(is);
	__sStar1.Read(is); __sStar2.Read(is);
	__sStar3.Read(is); __sStar4.Read(is);

	__G = cvCreateMat(__shape.nModes()+4, __texture.nPixels(), CV_64FC1);
	is >> __G;

	//alocate memory for on-line fitting stuff
	__Points = cvCreateMat (1, __shape.nPoints(), CV_64FC2);
	__Storage = cvCreateMemStorage(0);

	__update_s0 = cvCreateMat(1, __shape.nPoints()*2, CV_64FC1);
	__delta_s = cvCreateMat(1, __shape.nPoints()*2, CV_64FC1);
	__inv_pq = cvCreateMat(1, __shape.nModes()+4, CV_64FC1);
	__warp_t = cvCreateMat(1, __texture.nPixels(), CV_64FC1);
	__error_t = cvCreateMat(1, __texture.nPixels(), CV_64FC1);
	__search_pq = cvCreateMat(1, __shape.nModes()+4, CV_64FC1);
	__delta_pq = cvCreateMat(1, __shape.nModes()+4, CV_64FC1);
	__current_s = cvCreateMat(1, __shape.nPoints()*2, CV_64FC1);
	__update_s = cvCreateMat(1, __shape.nPoints()*2, CV_64FC1);
	__lamda  = cvCreateMat(1, __texture.nModes(), CV_64FC1);

}