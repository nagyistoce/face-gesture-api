#include "SourceLimiterNode.h"
#include "Tracer.h"
#include "LimitDecrementerBody.h"
#include "SourceNode.h"

using namespace std;
using namespace tbb::flow;


SourceLimiterNode::SourceLimiterNode(const VertexElement& vertexElement)
:	Node(vertexElement),
    limitDecrementerBody_(NULL),
    limitDecrementerNode_(NULL),
	limiterNode_(NULL),
    queueNode_(NULL)
{
    limitDecrementerBody_ = new LimitDecrementerBody(vertexElement);
}


SourceLimiterNode::~SourceLimiterNode(void)
{
    delete limitDecrementerBody_;
    delete limitDecrementerNode_;
	delete limiterNode_;
    delete queueNode_;
}


void SourceLimiterNode::BuildNode(const VertexContainer& modules)
{
    CollectPredecessors(modules);
    DefinePorts();
    CheckPorts();

    limitDecrementerNode_ = new MultiNodeContinueType(Node::graph, serial, *limitDecrementerBody_);
	limiterNode_ = new LimiterNodeType(Node::graph, 2);
    queueNode_ = new QueueNodeType(Node::graph);
}


void SourceLimiterNode::CreateEdge(void)
{
    string portName = inputPortNameMap_[LimitDecrementerBody::INPUT_IMAGE];
    ASSERT(predecessorMap_[portName]);
    SourceNode* sourceNodeIn = dynamic_cast<SourceNode*>(predecessorMap_[portName]);
    ASSERT(sourceNodeIn);
	Node::SourceNodeType* tbbSourceNodeIn = sourceNodeIn->GetSourceNode();
	ASSERT(tbbSourceNodeIn);

    //make_edge(*tbbSourceNodeIn, *queueNode_);
	//make_edge(*queueNode_, *limiterNode_);
	//make_edge(*limiterNode_, *limitDecrementerNode_);

    make_edge(*tbbSourceNodeIn, *limiterNode_);
	make_edge(*limiterNode_, *limitDecrementerNode_);

    make_edge(output_port<LimitDecrementerBody::OUTPUT_LIMITER>(*limitDecrementerNode_), limiterNode_->decrement);
}


void SourceLimiterNode::DefinePorts(void)
{
    inputPortNameMap_[LimitDecrementerBody::INPUT_IMAGE] = "image";
    outputPortNameMap_[LimitDecrementerBody::OUTPUT_DEFAULT] = "";
    outputPortNameMap_[LimitDecrementerBody::OUTPUT_LIMITER] = "limiter";
}


Node::LimiterNodeType* SourceLimiterNode::GetLimiterNode(void)
{
	return limiterNode_;
}


Node::MultiNodeContinueType* SourceLimiterNode::GetLimitDecrementerNode(void)
{
    return limitDecrementerNode_;
}
