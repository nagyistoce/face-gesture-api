#pragma once
#pragma warning(disable: 4251)

#include <map>
#include <vector>
#include <cstdarg>
#include "opencv2/opencv.hpp"

#include "DTW.h"

typedef std::vector<double> Movement;
typedef std::vector<Movement> MovementCluster;
typedef std::map<int, MovementCluster> MovementClusterBuffer;

extern "C++" class __declspec(dllexport) Similarity
{
public:
	enum CLUSTER_NAMES
	{
		NO_CLUSTER = 0,
		YES_CLUSTER,
        LEFT_CLUSTER,
        RIGHT_CLUSTER,
        TOP_CLUSTER,
        BOTTOM_CLUSTER,
        CIRCLE_CLUSTER,
        ZIGZAG_CLUSTER,
		UNKNOWN_CLUSTER = -1
	};
	Similarity(const std::string& databaseName, double diffThreshold);
	~Similarity(void);

	void Predict(const std::vector<double>& seq);
private:
    void PrintSeq(const std::vector<double>& seq);
	Movement CreateVector(int count, ...);

	void WriteDatabase(void);
	void ReadDatabase(void);

	MovementClusterBuffer movementClusterBuffer_;
	std::string databaseName_;
	DTW dtw_;

	double minDst_;
	double minErr_;
	int clusterLabel_;
	double diffThreshold_;
};

