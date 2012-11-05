#include "DTW.h"
#include "Visualizer.h"

using namespace std;
using namespace cv;

DTW::DTW(vector<double> &seq1, vector<double> &seq2) 
:	warpingPath_(seq1.size() + seq2.size(), vector<int>(2, 0)),
	K_(1),
	warpingDistance_(0.0)
{
	seq1_.assign(seq1.begin(), seq1.end());
	seq2_.assign(seq2.begin(), seq2.end());

	dtwImg_ = Mat(Size(seq1.size(), seq2.size()), CV_64FC1 );
}

DTW::~DTW(void)
{
}
	
void DTW::ComputeDTW(void) 
{
	double accumulatedDistance = 0.0;

	int n = seq1_.size();	
	int m = seq2_.size();
		
	vector<vector<double> > d(n, vector<double>(m, 0.0));	// local distances
	vector<vector<double> > D(n, vector<double>(m, 0.0));	// global distances

	// Typically: windowSize = Maximum(n, m) / 10.0.
	// If you set windowSize = Maximum(n, m), things will be slower.
	float windowSize = Maximum(n, m) / 10.0f;
		
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			d[i][j] = DistanceBetween(seq1_[i], seq2_[j]);
		
	D[0][0] = d[0][0];
		
	//for (int i = 1; i < n; i++)
	//	dtwImg_.at<float>(0, i) = D[i][0] = d[i][0] + D[i - 1][0];

	//for (int j = 1; j < m; j++) 
	//	dtwImg_.at<float>(j, 0) = D[0][j] = d[0][j] + D[0][j - 1];

	for (int i = 1; i < n; i++)
		for (int j = 1; j < m; j++) 
			dtwImg_.at<float>(j, i) = float(D[i][j] = d[i][j] + D[i - 1][j - 1]);
		
	for (int i = 1; i < n; i++) 
	{
		//for (int j = 1; j < m; j++) 
		for (int j = (int)Maximum(1, i - windowSize); j < (int)Minimum(m, i + windowSize + 1); j++) 
		{
			accumulatedDistance = Minimum(Minimum(float(D[i - 1][j]), float(D[i - 1][j-1])), float(D[i][j - 1]));
			accumulatedDistance += d[i][j];
			dtwImg_.at<float>(j, i) = float(accumulatedDistance);
			D[i][j] = accumulatedDistance;
		}
	}

	accumulatedDistance = D[n - 1][m - 1];

	int i = n - 1;
	int j = m - 1;
	int minIndex = 1;
	
	warpingPath_[K_ - 1][0] = i;
	warpingPath_[K_ - 1][1] = j;
		
	while ((i + j) != 0) 
	{
		if (i == 0) 
		{
			j -= 1;
		} 
		else if (j == 0) 
		{
			i -= 1;
		} 
		else // if( i != 0 && j != 0 )
		{	
			double a[] = { D[i - 1][j], D[i][j - 1], D[i - 1][j - 1] };
			minIndex = GetIndexOfMinimum(a, 3);

			if (minIndex == 0) 
				i -= 1;
			else if (minIndex == 1) 
				j -= 1;
			else if (minIndex == 2) 
				i -= 1, j -= 1;
		}

		K_++;
		warpingPath_[K_ - 1][0] = i;
		warpingPath_[K_ - 1][1] = j;
	}

	warpingDistance_ = accumulatedDistance / K_;
		
	ReversePath(warpingPath_);
}
	

void DTW::ReversePath(vector<vector<int> > path) 
{
	vector<vector<int> > newPath(K_, vector<int>(2, 0));

	for (int i = 0; i < K_; i++)
		for (int j = 0; j < 2; j++)
			newPath[i][j] = path[K_ - i - 1][j];

	warpingPath_ = newPath;
}

void DTW::ShowGrid(void)
{
	Mat dtwImg8;

	dtwImg_.convertTo(dtwImg8, CV_8U);

	for (int i = 0; i < K_; i++) 
		circle(dtwImg8, Point(warpingPath_[i][0], warpingPath_[i][1]), 1, Scalar(255.0), 1);

	//equalizeHist(dtwImg8, dtwImg8);
	resize(dtwImg8, dtwImg8, Size( 320, cvRound(dtwImg8.rows * (320.0 / dtwImg8.cols) ) ) );

	char text[100];
	sprintf_s(text, sizeof(text) - 1, "DTW distance: %.2lf",  warpingDistance_);

	putText(dtwImg8, text, Point(10, 20), CV_FONT_HERSHEY_COMPLEX, 0.5, Scalar(0), 2);
	putText(dtwImg8, text, Point(10, 20), CV_FONT_HERSHEY_COMPLEX, 0.5, Scalar(255), 1);

	VisualizerPtr->ShowImage("DTW Grid", dtwImg8);
}

double DTW::GetDistance(void) 
{
	return warpingDistance_;
}
	
double DTW::DistanceBetween(double p1, double p2) 
{	
	return (p1 - p2) * (p1 - p2);
}

int DTW::GetIndexOfMinimum(double* a, int n) 
{
	int index = 0;
	double val = a[0];

	for (int i = 1; i < n; i++) 
	{
		if (a[i] < val) 
		{
			val = a[i];
			index = i;
		}
	}
		
	return index;
}
