#include "Definitions.h"
#include "Similarity.h"

#include "Visualizer.h"
#include "LocalSettings.h"

using namespace std;
using namespace cv;

Similarity::Similarity(void)
{
	MovementCluster noCluster;
	Movement movement = CreateVector(18, 90, 90, 90, 90, 90, 90, 90, 270, 270, 270, 270, 270, 270, 270, 270, 225, 135, 180);
	noCluster.push_back(movement);

	movement = CreateVector(19, 90, 90, 90, 90, 90, 225, 270, 270, 270, 270, 270, 270, 270, 270, 270, 270, 225, 135, 90);
	noCluster.push_back(movement);

	movement = CreateVector(15, 90, 90, 90, 90, 90, 270, 270, 270, 270, 270, 270, 270, 225, 90, 90);
	noCluster.push_back(movement);

	movement = CreateVector(14, 90, 90, 90, 90, 90, 270, 270, 270, 270, 270, 135, 90, 135, 135);
	noCluster.push_back(movement);

	movement = CreateVector(12, 90, 90, 90, 90, 270, 270, 270, 270, 270, 180, 45, 90);
	noCluster.push_back(movement);

	movement = CreateVector(14, 90, 90, 90, 90, 270, 270, 270, 270, 270, 270, 270, 90, 90, 90);
	noCluster.push_back(movement);

	movementClusterBuffer_[NO_CLUSTER] = noCluster;
}


Similarity::~Similarity(void)
{
}


void Similarity::Predict(const vector<double>& seq)
{
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
			//cout << "DTW: " << dst << ", " << err << endl;
		}

		avgDst /= itMap->second.size();
		avgErr /= itMap->second.size();
		cout << "DTW (" << itMap->first << "): " << avgDst << ", " << avgErr << endl;
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
