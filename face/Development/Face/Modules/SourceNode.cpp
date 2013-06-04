#include "SourceNode.h"
#include "Tracer.h"
#include "SourceBody.h"

using namespace tbb::flow;


SourceNode::SourceNode(const ConnectionElement& connectionElement, tbb::flow::graph* graph)
:	graph_(*graph),
	sourceBody_(NULL),
	sourceNode_(NULL),
	limiterNode_(NULL)
{
	sourceBody_ = new SourceBody(connectionElement);
}


SourceNode::~SourceNode(void)
{
	delete sourceBody_;
	delete sourceNode_;
	delete limiterNode_;
}


void SourceNode::Start(void)
{
	sourceBody_->Start();
}
