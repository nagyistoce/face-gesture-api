#include "AAM_Basic.h"

#define MAG(a, b) sqrt((a) * (a) + (b) * (b))
#define THETA(a, b) atan((b) / (a))
#define curtime cvGetTickCount() / (cvGetTickFrequency() * 1000.0)

using namespace std;
//typedef unsigned char byte;

AAM_Basic::AAM_Basic(void)
:	AAM()
{
	__Rc = 0;
	__Rq = 0;

	__current_c = 0;
	__update_c = 0;
	__delta_c = 0;
	__p = 0;
	__current_q = 0;
	__update_q = 0;
	__delta_q = 0;
	__current_lamda = 0;
	__current_s = 0;
	__t_m = 0;
	__t_s = 0;
	__delta_t = 0;
}

AAM_Basic::~AAM_Basic(void)
{
	cvReleaseMat(&__Rc);
	cvReleaseMat(&__Rq);

	cvReleaseMat(&__current_c);
	cvReleaseMat(&__update_c);
	cvReleaseMat(&__delta_c);
	cvReleaseMat(&__p);
	cvReleaseMat(&__current_q);
	cvReleaseMat(&__update_q);
	cvReleaseMat(&__delta_q);
	cvReleaseMat(&__current_lamda);
	cvReleaseMat(&__current_s);
	cvReleaseMat(&__t_s);
	cvReleaseMat(&__t_m);
	cvReleaseMat(&__delta_t);

	//delete prevShape_;
}

//============================================================================
void AAM_Basic::InitParams(const IplImage* image) 
{
	//shape parameter
	__cam.__shape.CalcParams(__current_s, __p, __current_q);

	//texture parameter
	__cam.__paw.CalcWarpTexture(__current_s, image, __t_s);
	__cam.__texture.AlignTextureToRef(__cam.__MeanG, __t_s);
	__cam.__texture.CalcParams(__t_s, __current_lamda);

	//combined appearance parameter
	__cam.CalcParams(__current_c, __p, __current_lamda);
}

//============================================================================
void AAM_Basic::Fit(const IplImage* image, AAM_Shape& Shape, int max_iter, bool showprocess)
{
	//intial some stuff
	const int np = 6;
	double t = curtime, e1, e2;
	double k_v[np] = {1.0, 0.5, 0.25, 0.125, 0.0625, 0.0};
	
	Shape.Point2Mat(__current_s);
	InitParams(image);

	//calculate error
	e1 = EstResidual(image, __current_c, __current_s, __current_q, __delta_t);

	int iter = 0;

	//do a number of iteration until convergence
	for( ; iter < max_iter; iter++)
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

		// predict pose and parameter update
		cvGEMM(__delta_t, __Rq, 1, NULL, 0, __delta_q, CV_GEMM_B_T);
		cvGEMM(__delta_t, __Rc, 1, NULL, 0, __delta_c, CV_GEMM_B_T);

		// force first iteration
		if(iter == 0)
		{
			cvAdd(__current_q, __delta_q,  __current_q);
			cvAdd(__current_c, __delta_c,  __current_c);
			__cam.Clamp(__current_c);//constrain parameters
			e1 = EstResidual(image, __current_c, __current_s, __current_q, __delta_t);
		}
		// find largest step size which reduces texture EstResidual
		else
		{
			int k = 0;

			for( ; k < np; k++)
			{
				cvScaleAdd(__delta_q, cvScalar(k_v[k]), __current_q,  __update_q);
				cvScaleAdd(__delta_c, cvScalar(k_v[k]), __current_c,  __update_c);
				__cam.Clamp(__update_c);//constrain parameters				
				e2 = EstResidual(image, __update_c, __current_s, __update_q, __delta_t);

				if(e2 <= e1)	
					break;
			}

			// check for convergence
			if(k == np)
			{
				e1 = e2;
				cvCopy(__update_q, __current_q);
				cvCopy(__update_c, __current_c);
			}
			else if(fabs(e2 - e1) < 0.001 * e1)	
				break;
			else if(cvNorm(__delta_c) < 0.001 && cvNorm(__delta_q) < 0.001)	
				break;
			else
			{
				cvCopy(__update_q, __current_q);
				cvCopy(__update_c, __current_c);
				e1 = e2;
			}
		}
	}

	__cam.CalcShape(__current_s, __current_c, __current_q); 

	//if(prevShape_ == NULL)
	//	prevShape_ = new AAM_Shape(Shape);
	//else if(prevShape_->equalTo(Shape) == false)
	//	SmoothImagePoints(Shape);

	Shape.Point2Mat(__current_s);
	t = curtime - t;

	cout << "AAM-Basic Fitting time cost: " << t << " millisec." << endl;
	cout << "Number of iterations: " << iter << "." << endl;
}

//============================================================================
double AAM_Basic::EstResidual(const IplImage* image, const CvMat* c, CvMat* s, CvMat* q, CvMat* diff)
{
	// generate model texture
	__cam.CalcTexture(__t_m, c);
	
	// generate model shape
	__cam.CalcShape(s, c, q);    

	//calculate warped texture
	AAM_Basic::CheckShape(s, image->width, image->height);
	__cam.__paw.CalcWarpTexture(s, image, __t_s);
	__cam.__texture.AlignTextureToRef(__cam.__MeanG, __t_s);

	//calc pixel difference: g_s - g_m 
	cvSub(__t_m, __t_s, diff);

	return cvNorm(diff);
}

//===========================================================================
//bool AAM_Basic::CheckShape(const CvMat* s, int w, int h)
//{
//	double* fasts = s->data.db;
//	int npoints = s->cols / 2;
//
//	for(int i = 0; i < npoints; i++)
//	{
//		if(fasts[2*i] > w-1 || fasts[2*i] < 0)
//			return false;
//		if(fasts[2*i+1] > h-1 || fasts[2*i+1] < 0)
//			return false;
//	}
//	return true;
//}

// TODO: Így kicsit lassabb. Visszarakni boolra.
void AAM_Basic::CheckShape(const CvMat* s, int w, int h)
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

//===========================================================================
void AAM_Basic::Draw(IplImage* image, int type)
{
	if(type == 0) 
		DrawPoint(image);
	else if(type == 1) 
		DrawTriangle(image);
	else if(type == 2)	
		DrawAppearance(image);
	else 
		CV_Error(-1, "Unsupported AAM drawing type!");
}

//============================================================================
void AAM_Basic::DrawPoint(IplImage* image, bool drawNumbers)
{
	double* p = __current_s->data.db;

	for(int i = 0; i < __cam.__shape.nPoints(); i++)
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
void AAM_Basic::DrawTriangle(IplImage* image)
{
	double* p = __current_s->data.db;
	int idx1, idx2, idx3;
	for(int i = 0; i < __cam.__paw.nTri(); i++)
	{
		idx1 = __cam.__paw.__tri[i][0]; 
		idx2 = __cam.__paw.__tri[i][1];
		idx3 = __cam.__paw.__tri[i][2];

		cvLine(image, cvPoint(p[2*idx1], p[2*idx1+1]), cvPoint(p[2*idx2], p[2*idx2+1]),	CV_RGB(128,255,0));
		cvLine(image, cvPoint(p[2*idx2], p[2*idx2+1]), cvPoint(p[2*idx3], p[2*idx3+1]),	CV_RGB(128,255,0));
		cvLine(image, cvPoint(p[2*idx3], p[2*idx3+1]), cvPoint(p[2*idx1], p[2*idx1+1]),	CV_RGB(128,255,0));
	}
}


//============================================================================
void AAM_Basic::DrawAppearance(IplImage* image)
{
	AAM_Shape Shape; Shape.Mat2Point(__current_s);
	AAM_PAW paw;
	paw.Train(Shape, __cam.__Points, __cam.__Storage, __cam.__paw.GetTri(), false);
	int x1, x2, y1, y2, idx1, idx2;
	int tri_idx, v1, v2, v3;
	int xby3, idxby3;
	int minx, miny, maxx, maxy;
	AAM_Shape refShape;	refShape.Mat2Point(__cam.__MeanS);
	refShape.Translate(-refShape.MinX(), -refShape.MinY());
	double minV, maxV;
	cvMinMaxLoc(__t_m, &minV, &maxV);
	cvConvertScale(__t_m, __t_m, 255/(maxV-minV), -minV*255/(maxV-minV));
	unsigned char* pimg;
	double* fastt = __t_m->data.db;

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
		pimg = (unsigned char*)(image->imageData + image->widthStep*y);
		for(int x = minx; x < maxx; x++)
		{
			x1 = x-minx;
			idx1 = paw.__rect[y1][x1];
			if(idx1 >= 0)
			{
				tri_idx = paw.PixTri(idx1);
				v1 = paw.Tri(tri_idx, 0);
				v2 = paw.Tri(tri_idx, 1);
				v3 = paw.Tri(tri_idx, 2);
		
				x2 = paw.__alpha[idx1]*refShape[v1].x + paw.__beta[idx1]*refShape[v2].x +  
					paw.__gamma[idx1]*refShape[v3].x;
				y2 = paw.__alpha[idx1]*refShape[v1].y + paw.__beta[idx1]*refShape[v2].y +  
					paw.__gamma[idx1]*refShape[v3].y;
				
				xby3 = 3*x; 
				idx2 = __cam.__paw.__rect[y2][x2];		idxby3 = 3*idx2;
				pimg[xby3] = fastt[idxby3];
				pimg[xby3+1] = fastt[idxby3+1];
				pimg[xby3+2] = fastt[idxby3+2];
			}
		}
	}
}

//===========================================================================
void AAM_Basic::Read(std::ifstream& is, std::string& pModelFileName)
{
	cout << "Reading the Basic AAM Model from file '" << pModelFileName << "\' ..." << endl;

	__cam.Read(is);
	 __Rc = cvCreateMat(__cam.nModes(), __cam.__texture.nPixels(), CV_64FC1);
	 __Rq = cvCreateMat(4, __cam.__texture.nPixels(), CV_64FC1);
     is >> __Rc;
	 is >> __Rq;

	 //allocate memory for on-line fitting
	__current_c = cvCreateMat(1, __cam.nModes(), CV_64FC1);
	__update_c = cvCreateMat(1, __cam.nModes(), CV_64FC1);
	__delta_c = cvCreateMat(1, __cam.nModes(), CV_64FC1);
	__p = cvCreateMat(1, __cam.__shape.nModes(), CV_64FC1);
	__current_q = cvCreateMat(1, 4, CV_64FC1);
	__update_q = cvCreateMat(1, 4, CV_64FC1);
	__delta_q = cvCreateMat(1, 4, CV_64FC1);
	__current_lamda = cvCreateMat(1, __cam.__texture.nModes(), CV_64FC1);
	__current_s = cvCreateMat(1, __cam.__shape.nPoints()*2, CV_64FC1);
	__t_s = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
	__t_m = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
	__delta_t = cvCreateMat(1, __cam.__texture.nPixels(), CV_64FC1);
}
