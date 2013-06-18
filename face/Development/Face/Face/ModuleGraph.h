#pragma once

#include <opencv2/core/core.hpp>

#include "PortNameParser.h"

//! Module graph
/*!
	\ingroup ControlLayer

	Contains all of the modules in a single graph.
	Can be built from a cv::FileNode.
*/
class ModuleGraph
{
public:
	//! Constructor.
	ModuleGraph(void);

	//! Destructor.
	~ModuleGraph(void);

	//! Graph builder.
	/*!
		\param nodeProcesses The cv::FileNode which describes the modules and their connections.
	*/
	void BuildFromFileNode(const cv::FileNode& tasksNode);

private:
	void ReadConnectionListFromFileNode(const cv::FileNode& childrenNode, const PortNameParser& ppp, ChildrenList* list);
	bool NodeAlreadyExists(const PortNameParser& name) const;
    void CheckModules(void);

	VertexContainer modules_;
};
