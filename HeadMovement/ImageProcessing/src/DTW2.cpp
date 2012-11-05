#include "dolslr/doldef.h"
#include "dolslr/DTW2.h"
#include "opencv2/imgproc/imgproc.hpp"

#include "dolslr/Visualizer.h"

using namespace std;
using namespace cv;

DTW2::DTW2(void)
{
}

DTW2::DTW2(int n, int m)
:	n_(n), 
	m_(m),
	window_(int( Maximum(n, m) / 10.0 ))
{
	grid_ = new float* [n];

	for(int i = 0; i < n; i++)
		grid_[i] = new float[m];

	dtwImg_ = Mat(Size(n, m), CV_64FC1, FLT_MAX );
}

DTW2::~DTW2(void)
{
	for(int i = 0; i < n_; i++)
		delete[] grid_[i];

	delete[] grid_;
}

float DTW2::FastDTW(vector<double> &v, vector<double> &w, int distType, bool showGrid) 
{
	for (int i = 0; i < n_; i++)
		for (int j = 0; j < m_; j++)
			if(distType == EUCLIDEAN_DST)
				dtwImg_.at<float>(j, i) = grid_[i][j] = EuclideanDistance(v[i], w[j]);
			else if(distType == L1_DST)
				dtwImg_.at<float>(j, i) = grid_[i][j] = L1Distance(v[i], w[j]);
			else
				dtwImg_.at<float>(j, i) = grid_[i][j] = FLT_MAX;


	for (int i = 1; i < n_; i++) 
		for(int j = (int)Maximum(1.0f, i - window_); j < (int)Minimum(m_, i + window_ + 1.0f); j++) 
			if(distType == EUCLIDEAN_DST)
				dtwImg_.at<float>(j, i) = grid_[i][j] = EuclideanDistance(v[i], w[j]) + Minimum(grid_[i-1][j], Minimum(grid_[i][j-1], grid_[i-1][j-1]));
			else if(distType == L1_DST)
				dtwImg_.at<float>(j, i) = grid_[i][j] = L1Distance(v[i], w[j]) + Minimum(grid_[i-1][j], Minimum(grid_[i][j-1], grid_[i-1][j-1]));
			else
				dtwImg_.at<float>(j, i) = grid_[i][j] = FLT_MAX;

			if(showGrid)
			{	
				Mat dtwImg8;

				dtwImg_.convertTo(dtwImg8, CV_8U);
				resize(dtwImg8, dtwImg8, Size( 320.0, dtwImg8.rows * (320.0 / dtwImg8.cols) ) );

				char text[100];
				sprintf_s(text, sizeof(text) - 1, "DTW (%s) distance: %.2lf", distType == EUCLIDEAN_DST ? "Euclidean" : "L1",  grid_[n_ - 1][m_ - 1]);

				putText(dtwImg8, text, Point(10, 20), CV_FONT_HERSHEY_COMPLEX, 0.5, Scalar(0), 2);
				putText(dtwImg8, text, Point(10, 20), CV_FONT_HERSHEY_COMPLEX, 0.5, Scalar(255), 1);

				//dtwImg8.copyTo(VisualizerPtr->ShowFrame("DTW Grid - Old version"));
				//VisualizerPtr->ShowFrame("DTW Grid - Old version");
			}

			return grid_[n_ - 1][m_ - 1];
}

float DTW2::L1Distance(double p1, double p2) 
{
	return float(fabs(p1 - p2));
	//return fabs(pt1.x - pt2.x) + fabs(pt1.y - pt2.y);
}

float DTW2::EuclideanDistance(double p1, double p2) 
{
	return float((p1 - p2) * (p1 - p2));
	//return sqrt(pow(pt1.x - pt2.x, 2.0f) + pow(pt1.y - pt2.y, 2.0f));
}
