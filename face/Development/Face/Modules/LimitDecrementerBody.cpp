#include "LimitDecrementerBody.h"
#include "Tracer.h"
#include "ImageWrapper.h"

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
        imageWrapperIn_ = new ImageWrapper(*other.imageWrapperIn_);
}


LimitDecrementerBody::~LimitDecrementerBody(void)
{
}


void LimitDecrementerBody::operator() (const Body::InputType1& input, Node::MultiNodeContinueType::output_ports_type& output)
{
    imageWrapperIn_ = dynamic_cast<ImageWrapper*>(input);

    BeforeProcess();
    Process();
    AfterProcess();
    
    if(HasSuccessor())
    {
        std::get<OUTPUT_LIMITER>(output).try_put(tbb::flow::continue_msg());
    }
}


void LimitDecrementerBody::operator= (const LimitDecrementerBody& other)
{
    WARNING("Equality operator has been disabled.");
    return;
}


void LimitDecrementerBody::Process(void)
{
    ASSERT(imageWrapperIn_);

    GarbageCollectorPtr->InputHasBeenProcessed(imageWrapperIn_);

    GarbageCollector::GarbageItem* garbageItem = GarbageCollectorPtr->GetGarbageItem(imageWrapperIn_);
    if(garbageItem != NULL)
    {
        while(!std::get<GarbageCollector::PRESENT>(*garbageItem)) 
        {
            std::unique_lock<tbb::mutex> uniqueLock(*std::get<GarbageCollector::MUTEX>(*garbageItem));
            std::get<GarbageCollector::COND_VAR>(*garbageItem)->wait(uniqueLock);
        }

        ASSERT(std::get<GarbageCollector::REF_COUNT>(*garbageItem) == 0);
        GarbageCollectorPtr->EraseEntry(imageWrapperIn_);
    }

    delete imageWrapperIn_;
    imageWrapperIn_ = NULL;
}
