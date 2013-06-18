#include "dolslr/doldef.h"
#include "dolslr/SimMetrics.h"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace std;
using namespace cv;

SimMetrics::SimMetrics()
{
	graphW_ = 500;
	graphH_ = 210;
	imgGradT = Mat(graphH_ * 2, graphW_, CV_8UC3, Scalar(255, 255, 255));
	imgGradR = Mat(graphH_ * 2, graphW_, CV_8UC3, Scalar(255, 255, 255));

	imgDistRate = Mat(graphH_, graphW_, CV_8UC3, Scalar(255, 255, 255));
}

SimMetrics::~SimMetrics()
{
}

double SimMetrics::GetDistanceGr(double p1, double p2, double pd)
{
//	return sqrt(pow(p1 - p2, 2) + pow(pd, 2));
	return abs(p1 - p2);
	
}

double SimMetrics::DtwSeq(const vector<double>& seq1, const vector<double>& seq2, double& err)
{
	err = 0;

	if (seq1.size() < 100 || seq2.size() < 100)
		return -1.0;

	int m = seq1.size();
	int n = seq2.size();

	int minSize = m;
	if (minSize > n)
		minSize = n;

	vector<double> tmpseq1;
	vector<double> tmpseq2;

	if (minSize == m)
	{
		tmpseq1 = seq1;
		float q = (float)n / float(m);
		for (int i = 0; i < m; i++)
		{
			int t = i * q;
			tmpseq2.push_back(seq2[t]);
		}
	}
	else
	{
		tmpseq2 = seq2;
		float q = (float)m / (float)n;
		for (int i = 0; i < n; i++)
		{
			int t = i * q;
			tmpseq1.push_back(seq1[t]);
		}
	}
	

	vector<vector <double> > cost(minSize, minSize);

	cost[0][0] = GetDistanceGr(tmpseq1[0], tmpseq2[0], 0);
	
	for (int i = 1; i < minSize; i++)
		cost[i][0] = cost[i - 1][0] + GetDistanceGr(tmpseq1[i], tmpseq2[0], i);
		 
	
	for (int j = 1; j < minSize; j++)
		cost[0][j] = cost[0][j - 1] + GetDistanceGr(tmpseq1[0], tmpseq2[j], j);

	
	for (int j = 1; j < minSize; j++)
		for (int i = 1; i < minSize; i++)
			cost[i][j] = min(cost[i-1][j], min(cost[i][j-1], cost[i-1][j-1])) +  GetDistanceGr(tmpseq1[i], tmpseq2[j], i - j);
		

	dtwMap = Mat(minSize, minSize, CV_8UC3, Scalar(255,255,255));
	line(dtwMap, Point(0, 0), Point(minSize - 1, minSize - 1), Scalar(128, 128, 128), 1, 8, 0);

	int i = minSize - 1;
	int j = minSize - 1;

	double length = 0.0f;
	double d = 1.0f;
	double plt = 1;
	double fVal = 0;
	while ( (i + j) != 0)
	{
		dtwMap.at<Vec3b>(j,i)[0] = 0;
		dtwMap.at<Vec3b>(j,i)[1] = 0;
		dtwMap.at<Vec3b>(j,i)[2] = 0;
		d = 1;
		double minval;
		if (i == 0) 
		{
			j -= 1;	
			minval = cost[i][j];
		} 
		else 
			if (j == 0) 
			{
				i -= 1;
				minval = cost[i][j];
			} 
			else
			{
				double a = cost[i - 1][j];
				double b = cost[i][j - 1];
				double c = cost[i - 1][j - 1];

				minval = min(a, min(b, c));

				if (minval == a)
					if (a / c > 0.95)
					{
						minval = c;
						plt++;
					}

				if (minval == b)
					if (b / c > 0.95)
					{
						minval = c;
						plt++;
					}

				if (minval == a)
					i -= 1;
				else
					if (minval == b)
						j -= 1;
					else
					{
						i -= 1;
						j -= 1;
						d = 1.4142;
					}

				
			}
			length += d;

			fVal += minval;

	}

	
	err = ((length - sqrt(pow(minSize, 2.0) + pow(minSize, 2.0))) / length) * 100;
	
	return cost[minSize - 1][minSize - 1] / length;

//	return fVal / length;

}

double SimMetrics::FvdTransform(const vector<Point>& contour1, const vector<Point>& contour2)
{
	double disp = 0;

	vector<Point> normCont1;
	int size1 = contour1.size();
	if (size1 < 100)
		return disp;
	float q1 = (float)size1 / 100.0;

	for (int i = 0; i < size1; i++)
	{
		int t = i * q1;
		if (t < size1)
			normCont1.push_back(contour1[t]);	
	}



	vector<Point> normCont2;
	int size2 = contour2.size();
	if (size2 < 100)
		return disp;
	float q2 = (float)size2 / 100.0;

	for (int i = 0; i < size2; i++)
	{
		int t = i * q2;
		if (t < size2)
			normCont2.push_back(contour2[t]);	
	}



	int sizeN1 = 100;
	int sizeN2 = 100;

	float maxD1 = 0;
	for(int i = 0; i < sizeN1; i++)
	{
		float d = sqrt(pow(normCont1[0].x - normCont1[i].x, 2.0) + pow(normCont1[0].y - normCont1[i].y, 2.0));
		if (d > maxD1)
			maxD1 = d;
	}


	Mat imgDist1(sizeN1, sizeN1, CV_8UC1, Scalar(0));
	for(int j = 0; j < sizeN1; j++)
	{
		uchar *data = imgDist1.ptr<uchar>(j);
		for (int i = 0; i < sizeN1; i++)
		{
			*data++ = saturate_cast<uchar>(static_cast<int>(sqrt(pow(normCont1[j].x - normCont1[i].x, 2.0) + pow(normCont1[j].y - normCont1[i].y, 2.0)) / maxD1 * 100));

		}

	}



	float maxD2 = 0;
	for(int i = 0; i < sizeN2; i++)
	{
		float d = sqrt(pow(normCont2[0].x - normCont2[i].x, 2.0) + pow(normCont2[0].y - normCont2[i].y, 2.0));
		if (d > maxD2)
			maxD2 = d;
	}


	Mat imgDist2(sizeN2, sizeN2, CV_8UC1, Scalar(0));
	for(int j = 0; j < sizeN2; j++)
	{
		uchar *data = imgDist2.ptr<uchar>(j);
		for (int i = 0; i < sizeN2; i++)
		{
			*data++ = saturate_cast<uchar>(static_cast<int>(sqrt(pow(normCont2[j].x - normCont2[i].x, 2.0) + pow(normCont2[j].y - normCont2[i].y, 2.0)) / maxD2 * 100));

		}

	}


	Mat diff = abs(imgDist1 - imgDist2);

	float sumx = 0;
	float sumxn = 0;
	for(int j = 0; j < sizeN2; j++)
	{
		uchar *data = diff.ptr<uchar>(j);
		for (int i = 0; i < sizeN1; i++)
		{
			float v = *data++;

			sumx += v;
			sumxn += v*v;
		}
	}


	disp = sqrt((sizeN1 * sizeN2 * sumxn - pow(sumx, 2)) / pow(sizeN1 * sizeN2, 2.0));

	return disp;
}

double SimMetrics::GetSdgoValue(const vector<Point> &contour1, const vector<Point> &contour2, int id)
{

	// Normalizing contours
	vector<Point> normCont1;
	int size1 = contour1.size();
	float q = (float)size1 / 100.0;

	for (int i = 0; i < size1; i++)
	{
		int t = i * q;
		if (t < size1)
			normCont1.push_back(contour1[t]);
	}

	
	vector<Point> normCont2;
	int size2 = contour2.size();
	q = (float)size2 / 100.0;

	for (int i = 0; i < size2; i++)
	{
		int t = i * q;
		if (t < size2)
			normCont2.push_back(contour2[t]);	
	}


	// Preparing Tangential and Radial Signed Distance Gradient Orientation data sequences

	vector<int> gradSeqT1;
	vector<int> gradSeqT2;

	vector<int> gradSeqR1;
	vector<int> gradSeqR2;

	int sizeN1 = normCont1.size();
	int sizeN2 = normCont2.size();

	Moments moments1;
	moments1 = moments(normCont1, false);

	float cx1 = moments1.m10 / moments1.m00;
	float cy1 = moments1.m01 / moments1.m00;

	float dOldT = 0;
	float dOldR = sqrt(pow(cx1 - normCont1[0].x, 2) + pow(cy1 - normCont1[0].y, 2));
	for (int i = 1; i < sizeN1; i++)
	{
		float dNewT = sqrt(pow(normCont1[0].x - normCont1[i].x, 2.0) + pow(normCont1[0].y - normCont1[i].y, 2.0)); 
		float dNewR = sqrt(pow(cx1 - normCont1[i].x, 2) + pow(cy1 - normCont1[i].y, 2));

		if (dNewT - dOldT > 0)
			gradSeqT1.push_back(1);
		if (dNewT - dOldT < 0)
			gradSeqT1.push_back(-1);
		if (dNewT - dOldT == 0)
			gradSeqT1.push_back(0);


		if (dNewR - dOldR > 0)
			gradSeqR1.push_back(1);
		if (dNewR - dOldR < 0)
			gradSeqR1.push_back(-1);
		if (dNewR - dOldR == 0)
			gradSeqR1.push_back(0);


		dOldT = dNewT;
		dOldR = dNewR;
	}


	Moments moments2;
	moments2 = moments(normCont2, false);

	float cx2 = moments2.m10 / moments2.m00;
	float cy2 = moments2.m01 / moments2.m00;

	dOldT = 0;
	dOldR = sqrt(pow(cx2 - normCont2[0].x, 2) + pow(cy2 - normCont2[0].y, 2));
	for (int i = 1; i < sizeN2; i++)
	{
		float dNewT = sqrt(pow(normCont2[0].x - normCont2[i].x, 2.0) + pow(normCont2[0].y - normCont2[i].y, 2.0)); 
		float dNewR = sqrt(pow(cx2 - normCont2[i].x, 2) + pow(cy2 - normCont2[i].y, 2));

		if (dNewT - dOldT > 0)
			gradSeqT2.push_back(1);
		if (dNewT - dOldT < 0)
			gradSeqT2.push_back(-1);
		if (dNewT - dOldT == 0)
			gradSeqT2.push_back(0);

		if (dNewR - dOldR > 0)
			gradSeqR2.push_back(1);
		if (dNewR - dOldR < 0)
			gradSeqR2.push_back(-1);
		if (dNewR - dOldR == 0)
			gradSeqR2.push_back(0);

		dOldT = dNewT;
		dOldR = dNewR;
	}
/*
	// filtering noise
	for (int i = 1; i < gradSeqT1.size() - 1; i++)
		if (gradSeqT1[i - 1] * gradSeqT1[i] < 0 && gradSeqT1[i + 1] * gradSeqT1[i] < 0)
			gradSeqT1[i] = -gradSeqT1[i];

	for (int i = 1; i < gradSeqT2.size() - 1; i++)
		if (gradSeqT2[i - 1] * gradSeqT2[i] < 0 && gradSeqT2[i + 1] * gradSeqT2[i] < 0)
			gradSeqT2[i] = -gradSeqT2[i];

	for (int i = 1; i < gradSeqR1.size() - 1; i++)
		if (gradSeqR1[i - 1] * gradSeqR1[i] < 0 && gradSeqR1[i + 1] * gradSeqR1[i] < 0)
			gradSeqR1[i] = -gradSeqR1[i];

	for (int i = 1; i < gradSeqT2.size() - 1; i++)
		if (gradSeqR2[i - 1] * gradSeqR2[i] < 0 && gradSeqR2[i + 1] * gradSeqR2[i] < 0)
			gradSeqR2[i] = -gradSeqR2[i];	
*/

	line(imgGradT,Point(0, graphH_), Point(graphW_, graphH_), Scalar(0,0,0), 2, 8, 0);
	line(imgGradT, Point(0, graphH_ / 2), Point(graphW_, graphH_ / 2), Scalar(255, 0, 0), 1, 8, 0);
	float qT1 = (float)graphW_ / (float)gradSeqT1.size();
	for (int i = 0; i < gradSeqT1.size(); i++)
	{
		int px = i * qT1;
		int py = gradSeqT1[i] * 100;
		line(imgGradT, Point(px, (graphH_ / 2) - py), Point(px, (graphH_ / 2) - py), Scalar(0, 0, 255), 3, 8, 0);
	}

	if (id == 2)
	{
		line(imgGradT, Point(0, graphH_ + graphH_ / 2), Point(graphW_, graphH_ + graphH_ / 2), Scalar(255, 0, 0), 1, 8, 0);
		float qT2 = (float)graphW_ / (float)gradSeqT2.size();
		for (int i = 0; i < gradSeqT2.size(); i++)
		{
			int px = i * qT2;
			int py = gradSeqT2[i] * 100;
			line(imgGradT, Point(px, (graphH_ + graphH_ / 2) - py), Point(px, (graphH_ + graphH_ / 2) - py), Scalar(0, 0, 255), 3, 8, 0);
		}
	}
	

	// Comparing the gradient graphs

	int rotMax = gradSeqT2.size();

	vector<int> matchValT;
	vector<int> matchValR;


	// Tangential
	int minSize = min(gradSeqT1.size(), gradSeqT2.size());

	for(int n = 0; n < rotMax; n++)
	{
		int matchNum = 0;

		int rotIt = 1;
		if (n == 0)
			rotIt = 0;

		rotate(gradSeqT2.begin(), gradSeqT2.begin() + rotIt, gradSeqT2.end());

		for(int i = 0; i < minSize; i++)
		{
			if (gradSeqT1[i] - gradSeqT2[i] == 0)
				matchNum++;
		}

		matchValT.push_back(matchNum);
	}

	float maxVal = 0;
	int maxPos = 0;
	for (int i = 0; i < matchValT.size(); i++)
	{
		if (maxVal < matchValT[i])
		{
			maxVal = matchValT[i];
			maxPos = i;
		}
	}
	
	float simValT = maxVal / 100.0f;

	float sT = fabs(50.0 - maxPos) * 2.0 / 100.0 * simValT;


	// Radial
	minSize = min(gradSeqR1.size(), gradSeqR2.size());

	for(int n = 0; n < rotMax; n++)
	{
		int matchNum = 0;

		int rotIt = 1;
		if (n == 0)
			rotIt = 0;

		rotate(gradSeqR2.begin(), gradSeqR2.begin() + rotIt, gradSeqR2.end());

		for(int i = 0; i < minSize; i++)
		{
			if (gradSeqR1[i] - gradSeqR2[i] == 0)
				matchNum++;
		}

		matchValR.push_back(matchNum);
	}

	maxVal = 0;
	maxPos = 0;
	for (int i = 0; i < matchValR.size(); i++)
	{
		if (maxVal < matchValR[i])
		{
			maxVal = matchValR[i];
			maxPos = i;
		}
	}
	
	float simValR = maxVal / 100.0f;

	float sR = fabs(50.0 - maxPos) * 2.0 / 100.0 * simValR;


	float mixedVal = 1 - sqrt(simValT * simValR);
	

	float maxV = simValT;
	if (maxV < simValR)
		maxV = simValR;

	return (double) mixedVal;
	
}

double SimMetrics::GetRdrValue(const vector<Point> &contour1, const vector<Point> &contour2)
{

	// Normalizing contours
	vector<Point> normCont1;
	int size1 = contour1.size();
	float q = (float)size1 / 100.0;

	for (int i = 0; i < size1; i++)
	{
		int t = i * q;
		if (t < size1)
			normCont1.push_back(contour1[t]);
	}


	vector<Point> normCont2;
	int size2 = contour2.size();
	q = (float)size2 / 100.0;

	for (int i = 0; i < size2; i++)
	{
		int t = i * q;
		if (t < size2)
			normCont2.push_back(contour2[t]);	
	}

	// Preparing Radial Distance Rate data sequences

	vector<float> distSeqR1;
	vector<float> distSeqR2;

	vector<float> distSeqM1;
	vector<float> distSeqM2;

	int sizeN1 = normCont1.size();
	int sizeN2 = normCont2.size();

	Moments moments1;
	moments1 = moments(normCont1, false);

	float cx1 = moments1.m10 / moments1.m00;
	float cy1 = moments1.m01 / moments1.m00;

	float dNull = sqrt(pow(cx1 - normCont1[0].x, 2) + pow(cy1 - normCont1[0].y, 2));
	float dSum = 0;
	for (int i = 1; i < sizeN1; i++)
	{
		float dq = sqrt(pow(cx1 - normCont1[i].x, 2) + pow(cy1 - normCont1[i].y, 2)) / dNull;
		dSum += dq;
		distSeqR1.push_back(dq);
	}
	float meanDist1 = dSum / (float) sizeN1;



	Moments moments2;
	moments2 = moments(normCont2, false);

	float cx2 = moments2.m10 / moments2.m00;
	float cy2 = moments2.m01 / moments2.m00;

	dNull = sqrt(pow(cx2 - normCont2[0].x, 2) + pow(cy2 - normCont2[0].y, 2));
	dSum = 0;
	for (int i = 1; i < sizeN2; i++)
	{
		float dq = sqrt(pow(cx2 - normCont2[i].x, 2) + pow(cy2 - normCont2[i].y, 2)) / dNull;
		dSum += dq;
		distSeqR2.push_back(dq);
	}
	float meanDist2 = dSum / (float) sizeN2;


	for (int i = 0; i < distSeqR1.size(); i++)
		distSeqM1.push_back(distSeqR1[i] - meanDist1);

	for (int i = 0; i < distSeqR2.size(); i++)
		distSeqM2.push_back(distSeqR2[i] - meanDist2);




	// Comparing
	
	int rotMax = distSeqM2.size() / 2.5;

	int minSize = min(distSeqM1.size(), distSeqM2.size());

	vector<float> minDiff;
	vector<float> SQ;

	rotate(distSeqM2.begin(), distSeqM2.begin() + (distSeqM2.size() - rotMax / 2), distSeqM2.end());
	for(int n = 0; n < rotMax; n++)
	{
		int rotIt = 1;
		if (n == 0)
			rotIt = 0;

		rotate(distSeqM2.begin(), distSeqM2.begin() + rotIt, distSeqM2.end());

		float diffSum = 0;
		for (int i = 10; i < minSize - 10; i++)
		{
			float diff = fabs(distSeqM1[i] - distSeqM2[i]);
			diffSum += diff;
		}
		diffSum = diffSum / (float) minSize;

		minDiff.push_back(diffSum);								// shifted diffenrence sequence


		float sumSQ = 0;
		for (int i = 10; i < minSize - 10; i++)
		{
			float diff = fabs(distSeqM1[i] - distSeqM2[i]);
			sumSQ += pow(diff - diffSum, 2);
		}
		SQ.push_back(sumSQ);									// shifted sum of squared deviation
	}

	float minD = minDiff[0];
	float minDSQ = 0;
	int minPos = 0;
	int minInd = 0;
	for(int i = 1; i < minDiff.size(); i++)						// minimum difference
		if (minD > minDiff[i])
		{
			minD = minDiff[i];
			minPos = -(rotMax / 2) + i;
			minInd = i;
		}
	

	minDSQ = SQ[minInd];


	
	float minSQ = SQ[0];										// minimum of SQD
	for(int i = 1; i < SQ.size(); i++)
		if (minSQ > SQ[i])
			minSQ = SQ[i];			
	

	//return (double) minD;			// minimum difference
	//return (double) minDSQ;		// square sum of deviation at minimum difference
	return (double) minSQ;			// minimum of sum of deviation

}

double SimMetrics::GetRafValue(const vector<Point> &contour1, const vector<Point> &contour2)
{

	// Normalizing contours
	vector<Point> normCont1;
	int size1 = contour1.size();
	float q = (float)size1 / 100.0;

	for (int i = 0; i < size1; i++)
	{
		int t = i * q;
		if (t < size1)
			normCont1.push_back(contour1[t]);
	}


	vector<Point> normCont2;
	int size2 = contour2.size();
	q = (float)size2 / 100.0;

	for (int i = 0; i < size2; i++)
	{
		int t = i * q;
		if (t < size2)
			normCont2.push_back(contour2[t]);	
	}



	vector<float> angleSeq1;
	vector<float> angleSeq2;


	int sizeN1 = normCont1.size();
	int sizeN2 = normCont2.size();



	Moments moments1;
	moments1 = moments(normCont1, false);

	float cx1 = moments1.m10 / moments1.m00;
	float cy1 = moments1.m01 / moments1.m00;

	float px0 = cx1 - normCont1[0].x;
	float py0 = normCont1[0].y - cy1;

	for (int i = 1; i < sizeN1; i++)
	{
		float px1 = cx1 - normCont1[i].x;
		float py1 = normCont1[i].y - cy1;

		float px2 = px0 * px1 + py0 * py1;
		float py2 = py1 * px0 - px1 * py0;

		float angle = atan2(py2, px2) * 180 / CV_PI;

		if (angle < 0)
			angle += 360.0; 

		angleSeq1.push_back(angle);
	}

	

	Moments moments2;
	moments2 = moments(normCont2, false);

	float cx2 = moments2.m10 / moments2.m00;
	float cy2 = moments2.m01 / moments2.m00;

	px0 = cx2 - normCont2[0].x;
	py0 = normCont2[0].y - cy2;

	for (int i = 1; i < sizeN2; i++)
	{
		float px1 = cx2 - normCont2[i].x;
		float py1 = normCont2[i].y - cy2;

		float px2 = px0 * px1 + py0 * py1;
		float py2 = py1 * px0 - px1 * py0;

		float angle = atan2(py2, px2) * 180 / CV_PI;

		if (angle < 0)
			angle += 360.0; 

		angleSeq2.push_back(angle);
	}


	int minSize = min(angleSeq1.size(), angleSeq2.size());

	float diff = 0;
	for(int i = 10; i < minSize - 15; i++)
	{
		diff = diff + fabs(angleSeq1[i] - angleSeq2[i]);
	}

	diff = diff / minSize;

	

	return (double) diff;
	
}

double SimMetrics::CompareSeq(const vector<float> &seq1, vector<float> &seq2, float itRange, int methodType)
{
	

	int minSize = min(seq1.size(), seq2.size());

	int shiftRange = (float)minSize * (itRange / 100.0);
	
	vector<float> SQ;

	float minD = FLT_MAX;
	float minSQ = FLT_MAX;
	
	rotate(seq2.begin(), seq2.begin() + (seq2.size() - shiftRange / 2), seq2.end());
	
	for(int n = 0; n < shiftRange;)
	{
		int rotIt = 1;
		if (n == 0)
			rotIt = 0;

		rotate(seq2.begin(), seq2.begin() + rotIt, seq2.end());

		
		float diffSum = 0;
		
		
		for (int i = 10; i < minSize - 10; i++)
		{
			float diff = fabs(seq1[i] - seq2[i]);

			if (minD > diff)
				minD = diff;
				
			diffSum += diff;
		}
		
		diffSum = diffSum / (float) minSize;


		if (methodType == DEV_SQD)
		{
			float sumSQ = 0;
			for (int i = 10; i < minSize - 10; i++)
			{
				float diff = fabs(seq1[i] - seq2[i]);
				sumSQ += pow(diff - diffSum, 2);
			}
			
			if (minSQ > sumSQ)
				minSQ = sumSQ;
				
			
		}

	n += 1;
	}

	rotate(seq2.begin(), seq2.begin() + (seq2.size() - (shiftRange - 1) / 2), seq2.end());
	
	/*
	// drawing graphs
	imgDistRate.setTo(Scalar(255, 255, 255));
	line(imgDistRate, Point(0, graphH_ / 2), Point(graphW_, graphH_ / 2), Scalar(255, 0, 0));
	float qM1 = (float)graphW_ / (float)seq1.size();
	for (int i = 0; i < seq1.size(); i++)
	{
		int px = i * qM1;
		int py = seq1[i] * 100;
		line(imgDistRate, Point(px, (graphH_ / 2) - py), Point(px, (graphH_ / 2) - py), Scalar(0, 0, 255), 3, 8, 0);
	}

	
	float qM2 = (float)graphW_ / (float)seq2.size();
	for (int i = 0; i < seq2.size(); i++)
	{
		int px = i * qM2;
		int py = seq2[i] * 100;
		line(imgDistRate, Point(px, (graphH_ / 2) - py), Point(px, (graphH_ / 2) - py), Scalar(255, 0, 0), 3, 8, 0);
	}
	////
	*/


	if (methodType == DEV_LINEAR)
		return minD;
	

	if (methodType == DEV_SQD)
		return minSQ;


}