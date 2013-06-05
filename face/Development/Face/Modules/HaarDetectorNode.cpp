#include "HaarDetectorNode.h"
#include "Tracer.h"
#include "HaarDetectorBody.h"
#include "SourceNode.h"

using namespace std;
using namespace tbb::flow;


HaarDetectorNode::HaarDetectorNode(const VertexElement& vertexElement)
:	Node(vertexElement),
    haarDetectorBody_(NULL),
    functionNode_(NULL)
{
    haarDetectorBody_ = new HaarDetectorBody(vertexElement);
}


HaarDetectorNode::~HaarDetectorNode(void)
{
    delete haarDetectorBody_;
    delete functionNode_;
}


void HaarDetectorNode::BuildNode(const VertexContainer& modules)
{
    CollectPredecessors(modules);

    if(GetInstanceName() == "face")
        predecessorMap_["rectangle"] = NULL;

    DefinePorts();
    CheckPorts();

    functionNode_ = new FunctionNode1Type(Node::graph, tbb::flow::serial, *haarDetectorBody_);
}


void HaarDetectorNode::CreateEdge(void)
{
    string portName = inputPortNameMap_[HaarDetectorBody::INPUT_IMAGE];
    ASSERT(predecessorMap_[portName]);
    SourceNode* sourceIn = dynamic_cast<SourceNode*>(predecessorMap_[portName]);
    ASSERT(sourceIn);
    Node::LimiterNodeType* limiterNode = sourceIn->GetLimiterNode();
    ASSERT(limiterNode);

    make_edge(*limiterNode, *functionNode_);
    //make_edge(*limitDecrementer_, limiterNode->decrement);
}


void HaarDetectorNode::DefinePorts(void)
{
    inputPortNameMap_[HaarDetectorBody::INPUT_IMAGE] = "image";
    inputPortNameMap_[HaarDetectorBody::INPUT_RECTANGLE] = "rectangle";
    outputPortNameMap_[HaarDetectorBody::OUTPUT_DEFAULT] = "";
}
