#include "SourceNode.h"
#include "Tracer.h"
#include "SourceBody.h"

using namespace std;
using namespace tbb::flow;


SourceNode::SourceNode(const VertexElement& vertexElement)
:	Node(vertexElement),
	sourceBody_(NULL),
	sourceNode_(NULL)
{
	sourceBody_ = new SourceBody(vertexElement);
}


SourceNode::~SourceNode(void)
{
	delete sourceBody_;
	delete sourceNode_;
}


void SourceNode::BuildNode(const VertexContainer& modules)
{
    CollectPredecessors(modules);
    DefinePorts();
    CheckPorts();

    sourceNode_ = new SourceNodeType(Node::graph, *sourceBody_, false);
}


void SourceNode::CreateEdge(void)
{
}


void SourceNode::DefinePorts(void)
{
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
