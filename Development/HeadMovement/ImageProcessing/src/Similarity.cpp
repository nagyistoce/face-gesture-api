#include "Definitions.h"
#include "Similarity.h"

#include "Visualizer.h"
#include "LocalSettings.h"

using namespace std;
using namespace cv;


struct AvgDstVectorCmp 
{
    bool operator()(const AvgDstElem& lhs, const AvgDstElem& rhs) 
    {
        return lhs.second < rhs.second;
    }
};


Similarity::Similarity(const string& databaseName, double diffThreshold)
:	databaseName_(databaseName),
	diffThreshold_(diffThreshold),
	minDst_(DBL_MAX),
	minErr_(DBL_MAX),
	clusterLabel_(UNKNOWN_CLUSTER),
    procTime_(0.0)
{
	ReadDatabase();
}


Similarity::~Similarity(void)
{
	WriteDatabase();
}


void Similarity::Predict(const vector<double>& seq)
{
    procTime_ = (double)cvGetTickCount();

	minDst_ = DBL_MAX;
	minErr_ = DBL_MAX;
	clusterLabel_ = UNKNOWN_CLUSTER;

    avgDistances_.clear();

	MovementClusterBuffer::const_iterator itMap;
	for(itMap = movementClusterBuffer_.begin(); itMap != movementClusterBuffer_.end(); itMap++)
	{
		double avgDst = 0.0;
		double avgErr = 0.0;
		MovementCluster::const_iterator itMc;

		for(itMc = itMap->second.begin(); itMc != itMap->second.end(); itMc++)
		{
			double err = 0.0;
			double dst = dtw_.DoDtw(seq, *itMc, err);

			avgDst += dst;
			avgErr += err;
		}

		if(itMap->second.size() > 0)
		{
			avgDst /= itMap->second.size();
			avgErr /= itMap->second.size();
		}

        avgDistances_.push_back(make_pair(itMap->first, avgDst));

		if(avgDst > 0.0 && avgDst < minDst_)
		{
			minDst_ = avgDst;
			minErr_ = avgErr;
			clusterLabel_ = itMap->first;
		}
	}

    procTime_ = (double)cvGetTickCount() - procTime_;

    if(!avgDistances_.empty())
    {
        int barHeight = 30;
        double maxDst = 100.0;
        double stretch = 300.0;

        std::sort(avgDistances_.begin(), avgDistances_.end(), AvgDstVectorCmp());
        Mat output(barHeight * (avgDistances_.size()+1), 300, CV_8UC1, Scalar(0.0));

        stringstream ss;

        ss << "Processing time: " << procTime_ / (cvGetTickFrequency() * 1000.0) << " ms.";
        VisualizerPtr->PutText(output, ss.str(), Point(5, 20));

        for(int i = 0; i < int(avgDistances_.size()); i++)
        {
            int barWidth = cvRound(avgDistances_[i].second / maxDst * stretch);
            Rect r(0, (i+1) * barHeight, barWidth, barHeight);

            rectangle(output, r, i%2 ? Scalar(100.0) : Scalar(200.0), -1);

            rectangle(output, r, Scalar(barWidth%255), -1);
            rectangle(output, r, Scalar(255.0));

            stringstream ss;

            if(avgDistances_[i].first >= 97 && avgDistances_[i].first <= 122)
                ss << char(avgDistances_[i].first) << ": " << avgDistances_[i].second;
            else
                ss << avgDistances_[i].first << ": " << avgDistances_[i].second;

            VisualizerPtr->PutText(output, ss.str(), Point(5, (i+1) * barHeight + 20));
        }

        VisualizerPtr->ShowImage("DTW Distances", output);
    }

	if (minDst_ > 0.0 && minDst_ < diffThreshold_)
	{
		cout << "- DTW (" << clusterLabel_ << "): " << minDst_ << ", " << minErr_ << endl;
		movementClusterBuffer_[clusterLabel_].push_back(seq);
		WriteDatabase();
	}
	else
	{
		cout << "- DTW : " << minDst_ << ", " << minErr_ << endl;
	}

    //PrintSeq(seq);
}


void Similarity::PrintSeq(const vector<double>& seq)
{
    for (size_t i = 0; i < seq.size(); i++)
        cout << seq[i] << ". ";

    cout << endl;
}


Movement Similarity::CreateVector(int count, ...)
{
	va_list arguments;
	Movement retVector;

	va_start(arguments, count); 
	for(int i = 0; i < count; i++)
	{
		int ithAngle = va_arg(arguments, int);
		retVector.push_back(ithAngle);
	}
	va_end(arguments);

	return retVector;
}


void Similarity::WriteDatabase(void)
{
	FileStorage fs(LocalSettingsPtr->GetGestureDirectory() + databaseName_, FileStorage::WRITE);
	if(!fs.isOpened())
		CV_Error(1, "XML does not exist (" + LocalSettingsPtr->GetGestureDirectory() + databaseName_ + ")!");

	fs << "movement" << "[";
	
	MovementClusterBuffer::const_iterator itMap;
	for(itMap = movementClusterBuffer_.begin(); itMap != movementClusterBuffer_.end(); itMap++)
	{
		MovementCluster::const_iterator itMc;
		for(itMc = itMap->second.begin(); itMc != itMap->second.end(); itMc++)
			fs << "{:" << "id" << itMap->first << "angles" << *itMc << "}";
	}

	fs << "]";
	fs.release();
}


void Similarity::ReadDatabase(void)
{
	FileStorage fs(LocalSettingsPtr->GetGestureDirectory() + databaseName_, FileStorage::READ);
	if(!fs.isOpened())
		CV_Error(1, "XML does not exist (" + LocalSettingsPtr->GetGestureDirectory() + databaseName_ + ")!");

	FileNode node = fs["movement"];

	for(size_t i = 0; i < node.size(); ++i)
	{
		Movement movement;
		int id;

		node[i]["id"] >> id;
		node[i]["angles"] >> movement;

		movementClusterBuffer_[id].push_back(movement);
	}

	fs.release();
}
