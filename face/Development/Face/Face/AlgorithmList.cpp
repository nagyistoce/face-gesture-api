#include <iostream>

#include "AlgorithmList.h"
#include "Tracer.h"
#include "Algorithm.h"
#include "AlgorithmFactory.h"
#include "TestAlgorithm.h"

using namespace std;


AlgorithmList::AlgorithmList(void)
{
}


AlgorithmList::~AlgorithmList(void)
{
}


void AlgorithmList::BuildFromFileNode(const cv::FileNode& node)
{
	if(node.isSeq() == false) return;

	for(int i = 0; i < int(node.size()); ++i)
	{
		const cv::FileNode& processNode = node[i];
        Details details = {string(processNode["name"]), string(processNode["config"])};
		algorithmList_.push_back(details);

        AlgorithmFactoryPtr->Push(details.name, details.config);
	}
}
