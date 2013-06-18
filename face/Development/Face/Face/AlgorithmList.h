#pragma once

#include <string>
#include <vector>
#include <opencv2/core/core.hpp>

//! Algorithm list
/*!
	\ingroup ControlLayer

	Can create a thread, and execute a list of Algorithms (derived from the Algorithm class) 
	one after the other on this thread.

	The list of algorithms can be loaded from a cv::FileNode.
*/
class AlgorithmList 
{
public:
	AlgorithmList(void);
	virtual ~AlgorithmList(void);

	void BuildFromFileNode(const cv::FileNode& node);

protected:
	struct Details 
    {
		std::string name;
		std::string config;
	};

	std::vector<Details> algorithmList_;
};
