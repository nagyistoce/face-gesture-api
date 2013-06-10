#include "LimitDecrementerBody.h"
#include "Tracer.h"
#include "ImageWrapper.h"
#include "GarbageCollector.h"

using namespace std;


LimitDecrementerBody::LimitDecrementerBody(const VertexElement& vertexElement)
:   Body(vertexElement)
{
}


LimitDecrementerBody::LimitDecrementerBody(const LimitDecrementerBody& other)
:   Body(other)
{
}


LimitDecrementerBody::~LimitDecrementerBody(void)
{
}


void LimitDecrementerBody::operator() (const Body::InputType1& input, Node::MultiNodeContinueType::output_ports_type& output)
{
    ImageWrapper* imageWrapperIn = dynamic_cast<ImageWrapper*>(input);
    TRACE(GetFullName() + ": " + imageWrapperIn->GetMetaData().GetFrameNumber());

    BeforeProcess();
    Process();
    AfterProcess();
    
	GarbageCollectorPtr->SourceHasBeenProcessed(input);

	GarbageCollector::GarbageItem* garbageItem = GarbageCollectorPtr->GetGarbageItem(input);
	while(garbageItem && !std::get<GarbageCollector::PRESENT>(*garbageItem)) 
	{
		std::unique_lock<tbb::mutex> uniqueLock(*std::get<GarbageCollector::MUTEX>(*garbageItem));
		std::get<GarbageCollector::COND_VAR>(*garbageItem)->wait(uniqueLock);
	}

	GarbageCollectorPtr->EraseEntry(input);

    std::get<OUTPUT_LIMITER>(output).try_put(tbb::flow::continue_msg());
}


void LimitDecrementerBody::operator= (const LimitDecrementerBody& other)
{
    WARNING("Equality operator has been disabled.");
    return;
}


void LimitDecrementerBody::Process(void)
{

}
