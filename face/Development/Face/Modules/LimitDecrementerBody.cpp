#include "LimitDecrementerBody.h"
#include "Tracer.h"
#include "ImageMessage.h"
#include "GarbageCollector.h"

using namespace std;


LimitDecrementerBody::LimitDecrementerBody(const VertexElement& vertexElement)
:   Body(vertexElement),
    imageWrapperIn_(NULL)
{
}


LimitDecrementerBody::LimitDecrementerBody(const LimitDecrementerBody& other)
:   Body(other),
    imageWrapperIn_(NULL)
{
    if(other.imageWrapperIn_)
        imageWrapperIn_ = new ImageMessage(*other.imageWrapperIn_);
}


LimitDecrementerBody::~LimitDecrementerBody(void)
{
    delete imageWrapperIn_;
}


void LimitDecrementerBody::operator() (const Body::InputType1& input, Node::MultiNodeContinueType::output_ports_type& output)
{
    imageWrapperIn_ = dynamic_cast<ImageMessage*>(input);
    TRACE(GetFullName() + ": " + imageWrapperIn_->GetMetaData().GetFrameNumber());

    BeforeProcess();
    Process();
    AfterProcess();

    std::get<OUTPUT_LIMITER>(output).try_put(tbb::flow::continue_msg());
}


void LimitDecrementerBody::operator= (const LimitDecrementerBody& other)
{
    WARNING("Equality operator has been disabled.");
    return;
}


void LimitDecrementerBody::Process(void)
{
    GarbageCollectorPtr->InputHasBeenProcessed(imageWrapperIn_, GarbageCollector::NOTIFY_IF_PROCESSED);

    GarbageCollector::GarbageItem* garbageItem = GarbageCollectorPtr->GetGarbageItem(imageWrapperIn_);
    while(garbageItem && !std::get<GarbageCollector::PRESENT>(*garbageItem)) 
    {
        std::unique_lock<tbb::mutex> uniqueLock(*std::get<GarbageCollector::MUTEX>(*garbageItem));
        std::get<GarbageCollector::COND_VAR>(*garbageItem)->wait(uniqueLock);
    }

    GarbageCollectorPtr->EraseEntry(imageWrapperIn_);
}
