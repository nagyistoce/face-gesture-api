#include "SourceNode.h"
#include "Tracer.h"
#include "SourceBody.h"

using namespace std;
using namespace tbb::flow;


SourceNode::SourceNode(const VertexElement& vertexElement)
:	Node(vertexElement),
	sourceBody_(NULL),
	sourceNode_(NULL),
	limiterNode_(NULL)
{
	sourceBody_ = new SourceBody(vertexElement);
}


SourceNode::~SourceNode(void)
{
	delete sourceBody_;
	delete sourceNode_;
	delete limiterNode_;
}


void SourceNode::BuildNode(const VertexContainer& modules)
{
    CollectPredecessors(modules);
    DefinePorts();
    CheckPorts();

    sourceNode_ = new SourceNodeType(Node::graph, *sourceBody_, false);
    limiterNode_ = new LimiterNodeType(Node::graph, 10, 0);
}


void SourceNode::CreateEdge(void)
{
    make_edge(*sourceNode_, *limiterNode_);
}


void SourceNode::DefinePorts(void)
{
    portNameMap_[OUTPUT_DEFAULT] = "";
}


void SourceNode::RunNode(void)
{
	sourceBody_->Run();
    sourceNode_->activate();
}


Node::SourceNodeType* SourceNode::SourceNodeNode(void)
{
    return sourceNode_;
}


Node::LimiterNodeType* SourceNode::GetLimiterNode(void)
{
    return limiterNode_;
}
