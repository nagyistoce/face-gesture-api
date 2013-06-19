#include "AAMNode.h"
#include "Tracer.h"
#include "AAMBody.h"
#include "HaarDetectorBody.h"
#include "SourceLimiterNode.h"
#include "HaarDetectorNode.h"

using namespace std;
using namespace tbb::flow;


namespace face 
{
    AAMNode::AAMNode(const VertexElement& vertexElement)
    :	Node(vertexElement),
        AAMBody_(NULL),
        functionNode2_(NULL),
        joinNode2_(NULL)
    {
        AAMBody_ = new AAMBody(vertexElement);
    }


    AAMNode::~AAMNode(void)
    {
        delete AAMBody_;
        delete functionNode2_;
        delete joinNode2_;
    }


    void AAMNode::BuildNode(const VertexContainer& modules)
    {
        functionNode2_ = new FunctionNode2Type(Node::graph, tbb::flow::serial, *AAMBody_);
        joinNode2_ = new JoinNode2Type(Node::graph);
    }


    void AAMNode::CreateEdge(void)
    {
        // Create camera connection:
        string portName = inputPortNameMap_[AAMBody::INPUT_IMAGE];

        ASSERT(predecessorMap_[portName]);
        SourceLimiterNode* sourceLimiterIn = dynamic_cast<SourceLimiterNode*>(predecessorMap_[portName]);

        ASSERT(sourceLimiterIn);
        Node::LimiterNodeType* tbbSourceLimiterIn = sourceLimiterIn->GetLimiterNode();

        ASSERT(tbbSourceLimiterIn);
        make_edge(*tbbSourceLimiterIn, std::get<AAMBody::INPUT_IMAGE>(joinNode2_->input_ports()));


        // Create rectangle connection:
        portName = inputPortNameMap_[AAMBody::INPUT_RECTANGLE];
        
        ASSERT(predecessorMap_[portName]);
        HaarDetectorNode* haarDetectorIn = dynamic_cast<HaarDetectorNode*>(predecessorMap_[portName]);

        ASSERT(haarDetectorIn);
        Node::FunctionNode2Type* tbbHaarDetectorIn = haarDetectorIn->GetFunction2Node();

        ASSERT(tbbHaarDetectorIn);
        make_edge(*tbbHaarDetectorIn, std::get<AAMBody::INPUT_RECTANGLE>(joinNode2_->input_ports()));


        // JoinNode to AAM FunctionNode:
        make_edge(*joinNode2_, *functionNode2_);
    }


    void AAMNode::DefinePorts(void)
    {
        inputPortNameMap_[AAMBody::INPUT_IMAGE] = "image";
        inputPortNameMap_[AAMBody::INPUT_RECTANGLE] = "rectangle";
        outputPortNameMap_[AAMBody::OUTPUT_DEFAULT] = "";
    }


    Node::FunctionNode2Type* AAMNode::GetFunction2Node(void)
    {
        return functionNode2_;
    }
}
