//#include "HaarDetector.h"
//#include "Tracer.h"
//#include "ImageWrapper.h"
//#include "Source.h"
//
//using namespace std;
//
//
//HaarDetector::HaarDetector(const VertexElement& connectionElement)
//:   Module(connectionElement),
//    //functionNode_(NULL),
//    imageWrapperIn_(NULL)
//{
//    //continueNode_ = new ContinueNodeType(graph_);
//    //functionNode_ = new FunctionNodeType(graph_, tbb::flow::serial, *this);
//}
//
//
//HaarDetector::HaarDetector(const HaarDetector& other)
//:   Module(other)
//{
//    imageWrapperIn_ = other.imageWrapperIn_ ? new ImageWrapper(*other.imageWrapperIn_) : NULL;
//    //functionNode_ = other.functionNode_ ? new FunctionNodeType(*other.functionNode_) : NULL;
//}
//
//
//HaarDetector::~HaarDetector(void)
//{
//    //delete functionNode_;
//    delete imageWrapperIn_;
//}
//
//
//void HaarDetector::DefinePorts(void)
//{
//    portNameMap_[INPUT_IMAGE] = "image";
//    portNameMap_[INPUT_RECTANGLE] = "rectangle";
//    portNameMap_[OUTPUT_DEFAULT] = "";
//}
//
//
//TbbNode::OutputType HaarDetector::operator() (TbbNode::InputType1 input)
//{
//    //imageWrapperIn_ = dynamic_cast<ImageWrapper*>();
//
//    BeforeProcess();
//    Process();
//    AfterProcess();
//
//    return output_;
//}
//
//
//void HaarDetector::CreateConnection(Module::PredecessorMap& predecessorMap)
//{
//    //ASSERT(functionNode_);
//
//    DefinePorts();
//    CheckPorts(predecessorMap);
//
//    ASSERT(predecessorMap[portNameMap_[INPUT_IMAGE]]);
//    Source* sourceIn = dynamic_cast<Source*>(predecessorMap[portNameMap_[INPUT_IMAGE]]);
//    ASSERT(sourceIn);
//    //Source::LimiterNodeType* limiterNode = sourceIn->GetNode();
//    //ASSERT(limiterNode);
//    //tbb::flow::make_edge(*limiterNode, *functionNode_);
//    //tbb::flow::make_edge(*continueNode_, limiterNode->decrement);
//}
//
//
//void HaarDetector::Process(void)
//{
//    if(HasOutput())
//    {
//        //continueNode_->try_put(tbb::flow::continue_msg());
//    }
//
//    //if(continueNode_ == NULL)
//      //  puts("+++");
////   continueNode_->try_put(tbb::flow::continue_msg());
//}
//
//
////HaarDetector::FunctionNodeType* HaarDetector::GetNode(void)
////{
////    return functionNode_;
////}
