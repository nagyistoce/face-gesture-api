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
    limiterNode_ = new LimiterNodeType(Node::graph, 5/*, 0*/);
}


void SourceNode::CreateEdge(void)
{
    make_edge(*sourceNode_, *limiterNode_);
}


void SourceNode::DefinePorts(void)
{
    inputPortNameMap_[SourceBody::INPUT_LIMITER] = "limiter";
    outputPortNameMap_[SourceBody::OUTPUT_DEFAULT] = "";
}


void SourceNode::RunNode(void)
{
    sourceNode_->activate();
}


Node::SourceNodeType* SourceNode::GetSourceNode(void)
{
    return sourceNode_;
}


Node::LimiterNodeType* SourceNode::GetLimiterNode(void)
{
    return limiterNode_;
}
