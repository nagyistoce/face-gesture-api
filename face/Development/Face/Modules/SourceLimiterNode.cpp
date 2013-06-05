#include "SourceLimiterNode.h"
#include "Tracer.h"
#include "SourceLimiterBody.h"
#include "SourceNode.h"

using namespace std;
using namespace tbb::flow;


SourceLimiterNode::SourceLimiterNode(const VertexElement& vertexElement)
:	Node(vertexElement),
    sourceLimiterBody_(NULL),
    multiNodeContinue_(NULL)
{
    sourceLimiterBody_ = new SourceLimiterBody(vertexElement);
}


SourceLimiterNode::~SourceLimiterNode(void)
{
    delete sourceLimiterBody_;
    delete multiNodeContinue_;
}


void SourceLimiterNode::BuildNode(const VertexContainer& modules)
{
    CollectPredecessors(modules);
    DefinePorts();
    CheckPorts();

    multiNodeContinue_ = new MultiNodeContinueType(Node::graph, unlimited, *sourceLimiterBody_);
}


void SourceLimiterNode::CreateEdge(void)
{
    string portName = inputPortNameMap_[SourceLimiterBody::INPUT_IMAGE];
    ASSERT(predecessorMap_[portName]);
    SourceNode* sourceIn = dynamic_cast<SourceNode*>(predecessorMap_[portName]);
    ASSERT(sourceIn);
    Node::LimiterNodeType* limiterNode = sourceIn->GetLimiterNode();
    ASSERT(limiterNode);

    make_edge(output_port<SourceLimiterBody::OUTPUT_LIMITER>(*multiNodeContinue_), limiterNode->decrement);
    make_edge(*limiterNode, *multiNodeContinue_);
    //output_port<SourceLimiterBody::OUTPUT_LIMITER>(*multiNodeContinue_).register_successor(limiterNode->decrement);
    
}


void SourceLimiterNode::DefinePorts(void)
{
    inputPortNameMap_[SourceLimiterBody::INPUT_IMAGE] = "image";
    outputPortNameMap_[SourceLimiterBody::OUTPUT_DEFAULT] = "";
    outputPortNameMap_[SourceLimiterBody::OUTPUT_LIMITER] = "limiter";
}


Node::MultiNodeContinueType* SourceLimiterNode::GetMultiNodeContinue(void)
{
    return multiNodeContinue_;
}
