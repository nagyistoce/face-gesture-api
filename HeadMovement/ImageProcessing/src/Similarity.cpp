#include "Definitions.h"
#include "Similarity.h"

#include "Visualizer.h"
#include "LocalSettings.h"

using namespace std;
using namespace cv;

Similarity::Similarity(const string& databaseName, double diffThreshold)
:	databaseName_(databaseName),
	diffThreshold_(diffThreshold),
	minDst_(DBL_MAX),
	minErr_(DBL_MAX),
	clusterLabel_(UNKNOWN_CLUSTER)
{
	ReadDatabase();
}


Similarity::~Similarity(void)
{
	WriteDatabase();
}


void Similarity::Predict(const vector<double>& seq)
{
	minDst_ = DBL_MAX;
	minErr_ = DBL_MAX;
	clusterLabel_ = UNKNOWN_CLUSTER;

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

		if(avgDst > 0.0 && avgDst < minDst_)
		{
			minDst_ = avgDst;
			minErr_ = avgErr;
			clusterLabel_ = itMap->first;
		}
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
