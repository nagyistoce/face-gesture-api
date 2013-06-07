#include "EmptyNode.h"
#include "Tracer.h"
#include "EmptyBody.h"
#include "SourceLimiterNode.h"

using namespace std;
using namespace tbb::flow;


EmptyNode::EmptyNode(const VertexElement& vertexElement)
:	Node(vertexElement),
    emptyBody_(NULL),
    functionNode_(NULL)
{
    emptyBody_ = new EmptyBody(vertexElement);
}


EmptyNode::~EmptyNode(void)
{
    delete emptyBody_;
    delete functionNode_;
}


void EmptyNode::BuildNode(const VertexContainer& modules)
{
    CollectPredecessors(modules);
    DefinePorts();
    CheckPorts();

    functionNode_ = new FunctionNode1Type(Node::graph, tbb::flow::serial, *emptyBody_);
}


void EmptyNode::CreateEdge(void)
{
    string portName = inputPortNameMap_[EmptyBody::INPUT_IMAGE];
    ASSERT(predecessorMap_[portName]);
    SourceLimiterNode* sourceLimiterIn = dynamic_cast<SourceLimiterNode*>(predecessorMap_[portName]);
    ASSERT(sourceLimiterIn);
    Node::LimiterNodeType* tbbSourceLimiterIn = sourceLimiterIn->GetLimiterNode();
    ASSERT(tbbSourceLimiterIn);

    make_edge(*tbbSourceLimiterIn, *functionNode_);
}


void EmptyNode::DefinePorts(void)
{
    inputPortNameMap_[EmptyBody::INPUT_IMAGE] = "image";
    outputPortNameMap_[EmptyBody::OUTPUT_DEFAULT] = "";
}


Node::FunctionNode1Type* EmptyNode::GetFunctionNode(void)
{
    return functionNode_;
}
