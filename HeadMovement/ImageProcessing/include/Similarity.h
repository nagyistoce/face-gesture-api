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
		YES_CLUSTER
	};
	Similarity(void);
	~Similarity(void);

	void Predict(const std::vector<double>& seq);
private:
	Movement CreateVector(int count, ...);

	MovementClusterBuffer movementClusterBuffer_;
	DTW dtw_;
};

