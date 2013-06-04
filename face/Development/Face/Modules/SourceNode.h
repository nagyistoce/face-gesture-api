#pragma once

#include <tbb/flow_graph.h>

#include "FaceDef.h"
#include "PortNameParser.h"


class SourceBody;

class SourceNode
{
public:
	SourceNode(const ConnectionElement& connectionElement, tbb::flow::graph* graph);
	~SourceNode(void);

	void Start(void);


private:
	tbb::flow::graph& graph_;
	SourceBody* sourceBody_;
	SourceNodeType* sourceNode_;
	LimiterNodeType* limiterNode_;
};

