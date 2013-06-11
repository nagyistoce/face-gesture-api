#include "HaarDetectorNode.h"
#include "Tracer.h"
#include "HaarDetectorBody.h"
#include "EmptyBody.h"
#include "SourceLimiterNode.h"
#include "EmptyNode.h"

using namespace std;
using namespace tbb::flow;


HaarDetectorNode::HaarDetectorNode(const VertexElement& vertexElement)
:	Node(vertexElement),
    haarDetectorBody_(NULL),
    functionNode2_(NULL),
    joinNode2_(NULL)
{
    haarDetectorBody_ = new HaarDetectorBody(vertexElement);
}


HaarDetectorNode::~HaarDetectorNode(void)
{
    delete haarDetectorBody_;
    delete functionNode2_;
    delete joinNode2_;
}


void HaarDetectorNode::BuildNode(const VertexContainer& modules)
{
    functionNode2_ = new FunctionNode2Type(Node::graph, tbb::flow::serial, *haarDetectorBody_);
    joinNode2_ = new JoinNode2Type(Node::graph);
}


void HaarDetectorNode::CreateEdge(void)
{
    string portName = inputPortNameMap_[HaarDetectorBody::INPUT_IMAGE];
    ASSERT(predecessorMap_[portName]);
    SourceLimiterNode* sourceLimiterIn = dynamic_cast<SourceLimiterNode*>(predecessorMap_[portName]);
    ASSERT(sourceLimiterIn);
    Node::LimiterNodeType* tbbSourceLimiterIn = sourceLimiterIn->GetLimiterNode();
    ASSERT(tbbSourceLimiterIn);

    make_edge(*tbbSourceLimiterIn, std::get<HaarDetectorBody::INPUT_IMAGE>(joinNode2_->input_ports()));

    portName = inputPortNameMap_[HaarDetectorBody::INPUT_RECTANGLE];
    ASSERT(predecessorMap_[portName]);

    if(GetInstanceName() == "face")
    {
        EmptyNode* emptyNode = dynamic_cast<EmptyNode*>(predecessorMap_[portName]);
        ASSERT(emptyNode);
        Node::FunctionNode1Type* tbbEmptyRectIn = emptyNode->GetFunctionNode();
        ASSERT(tbbEmptyRectIn);
        make_edge(*tbbEmptyRectIn, std::get<HaarDetectorBody::INPUT_RECTANGLE>(joinNode2_->input_ports()));
    }
    else
    {
        HaarDetectorNode* haarDetectorIn = dynamic_cast<HaarDetectorNode*>(predecessorMap_[portName]);
        ASSERT(haarDetectorIn);
        Node::FunctionNode2Type* tbbHaarDetectorIn = haarDetectorIn->GetFunction2Node();
        ASSERT(tbbHaarDetectorIn);
        make_edge(*tbbHaarDetectorIn, std::get<HaarDetectorBody::INPUT_RECTANGLE>(joinNode2_->input_ports()));
    }

    make_edge(*joinNode2_, *functionNode2_);
}


void HaarDetectorNode::DefinePorts(void)
{
    inputPortNameMap_[HaarDetectorBody::INPUT_IMAGE] = "image";
    inputPortNameMap_[HaarDetectorBody::INPUT_RECTANGLE] = "rectangle";
    outputPortNameMap_[HaarDetectorBody::OUTPUT_DEFAULT] = "";
}


Node::FunctionNode2Type* HaarDetectorNode::GetFunction2Node(void)
{
    return functionNode2_;
}
