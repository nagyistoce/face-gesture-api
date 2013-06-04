#include "HaarDetectorNode.h"
#include "Tracer.h"
#include "HaarDetectorBody.h"
#include "SourceNode.h"

using namespace std;
using namespace tbb::flow;


HaarDetectorNode::HaarDetectorNode(const VertexElement& vertexElement)
:	Node(vertexElement),
    haarDetectorBody_(NULL),
    functionNode_(NULL),
    limitDecrementer_(NULL)
{
    haarDetectorBody_ = new HaarDetectorBody(vertexElement);
}


HaarDetectorNode::~HaarDetectorNode(void)
{
    delete haarDetectorBody_;
    delete functionNode_;
    delete limitDecrementer_;
}


void HaarDetectorNode::BuildNode(const VertexContainer& modules)
{
    CollectPredecessors(modules);

    if(GetInstanceName() == "face")
        predecessorMap_["rectangle"] = NULL;

    DefinePorts();
    CheckPorts();

    limitDecrementer_ = new BroadcastContinueNodeType(Node::graph);
    functionNode_ = new FunctionNode1Type(Node::graph, tbb::flow::serial, *haarDetectorBody_);
}


void HaarDetectorNode::CreateEdge(void)
{
    ASSERT(predecessorMap_[portNameMap_[INPUT_IMAGE]]);
    SourceNode* sourceIn = dynamic_cast<SourceNode*>(predecessorMap_[portNameMap_[INPUT_IMAGE]]);
    ASSERT(sourceIn);
    Node::LimiterNodeType* limiterNode = sourceIn->GetLimiterNode();
    ASSERT(limiterNode);

    make_edge(*limiterNode, *functionNode_);
    make_edge(*limitDecrementer_, limiterNode->decrement);
}


void HaarDetectorNode::DefinePorts(void)
{
    portNameMap_[INPUT_IMAGE] = "image";
    portNameMap_[INPUT_RECTANGLE] = "rectangle";
    portNameMap_[OUTPUT_DEFAULT] = "";
}
