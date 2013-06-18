#include "DTW.h"
#include "Visualizer.h"

using namespace std;
using namespace cv;

DTW::DTW(void)
:   procTime_(0.0)
{
}

DTW::~DTW(void)
{
}

double DTW::DoDtw(const vector<double>& seq1, const vector<double>& seq2, double& err)
{
    procTime_ = (double)cvGetTickCount();

    err = 0;

    if (seq1.size() < 5 || seq2.size() < 5)
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
            int t = cvRound(i * q);
            tmpseq2.push_back(seq2[t]);
        }
    }
    else
    {
        tmpseq2 = seq2;
        float q = (float)m / (float)n;
        for (int i = 0; i < n; i++)
        {
            int t = cvRound(i * q);
            tmpseq1.push_back(seq1[t]);
        }
    }


    vector<vector<double> > cost(minSize, minSize);

    cost[0][0] = Distance(tmpseq1[0], tmpseq2[0], 0);

    for(int i = 1; i < minSize; i++)
        cost[i][0] = cost[i - 1][0] + Distance(tmpseq1[i], tmpseq2[0], i);


    for(int j = 1; j < minSize; j++)
        cost[0][j] = cost[0][j - 1] + Distance(tmpseq1[0], tmpseq2[j], j);


    for(int j = 1; j < minSize; j++)
        for (int i = 1; i < minSize; i++)
            cost[i][j] = min(cost[i-1][j], min(cost[i][j-1], cost[i-1][j-1])) +  Distance(tmpseq1[i], tmpseq2[j], i - j);


    dtwMap_ = Mat(minSize, minSize, CV_8UC3, Scalar(255,255,255));
    line(dtwMap_, Point(0, 0), Point(minSize - 1, minSize - 1), Scalar(128, 128, 128), 1, 8, 0);

    int i = minSize - 1;
    int j = minSize - 1;

    double length = 0.0f;
    double d = 1.0f;
    double plt = 1;
    double fVal = 0;
    while ( (i + j) != 0)
    {
        dtwMap_.at<Vec3b>(j,i)[0] = 0;
        dtwMap_.at<Vec3b>(j,i)[1] = 0;
        dtwMap_.at<Vec3b>(j,i)[2] = 0;
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
    procTime_ = (double)cvGetTickCount() - procTime_;

    // Visualizer
    //if(draw)
    //{
        //double ratio = 300.0 / dtwMap_.cols;
        //resize(dtwMap_, dtwMap_, Size(300, cvRound(dtwMap_.rows * ratio)), 0.0, 0.0, cv::INTER_AREA);
        //
        //stringstream ss;

        //ss << "Processing time: " << procTime_ / (cvGetTickFrequency() * 1000.0) << " ms.";
        //VisualizerPtr->PutText(dtwMap_, ss.str(), Point(10, 20));
        //ss.str("");

        //ss << "DTW distance: " << (cost[minSize - 1][minSize - 1] / length);
        //VisualizerPtr->PutText(dtwMap_, ss.str(), Point(10, 40));
        //ss.str("");

        //VisualizerPtr->ShowImage("DTW Grid", dtwMap_);
    //}

    return cost[minSize - 1][minSize - 1] / length;
    //return fVal / length;
}

double DTW::Distance(double p1, double p2, double pd)
{
    //return sqrt(pow(p1 - p2, 2) + pow(pd, 2));
    return abs(p1 - p2);
}
