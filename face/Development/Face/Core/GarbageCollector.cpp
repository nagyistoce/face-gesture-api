#include "GarbageCollector.h"
#include "Tracer.h"
#include "Message.h"

GarbageCollector* GarbageCollector::instance_ = NULL;
tbb::mutex GarbageCollector::mutex_;


GarbageCollector* GarbageCollector::GetInstance(void)
{
    if(instance_ == NULL)
    {
        tbb::mutex::scoped_lock lock(mutex_);
        if(instance_ == NULL)
            instance_ = new GarbageCollector();
    }
    return instance_;
}


GarbageCollector::GarbageCollector(void)
{
}


GarbageCollector::~GarbageCollector(void)
{
	for(GarbageContainer::iterator it = garbageContainer_.begin(); it != garbageContainer_.end(); it++)
	{
		delete std::get<COND_VAR>(it->second);
		delete std::get<MUTEX>(it->second);
	}

	garbageContainer_.clear();
}


void GarbageCollector::ParseConnectionMap(const VertexContainer& modules)
{
    ASSERT(inputMap_.empty());

	for(VertexContainer::const_iterator it = modules.begin(); it != modules.end(); it++)
	{
		inputMap_[it->first.GetFullName()] = 0;
	}

    for(VertexContainer::const_iterator it = modules.begin(); it != modules.end(); it++)
    {
		if(it->first.GetModuleName() == "SourceLimiter")
			inputMap_["Source." + it->first.GetInstanceName()] += it->second.size();

        inputMap_[it->first.GetFullName()] += it->second.size();
    }
}


void GarbageCollector::PushNewOutput(Message* newOutput, const std::string& moduleFullName)
{
    {
        tbb::mutex::scoped_lock lock(mutex_);

		if(!newOutput || garbageContainer_.find(newOutput) != garbageContainer_.end())
			return;

        garbageContainer_[newOutput] = std::make_tuple(
			inputMap_[moduleFullName], 
			new tbb::mutex(),
			new std::condition_variable(),
			false
		);
    }
}


void GarbageCollector::InputHasBeenProcessed(Message* input, int strategy)
{
    tbb::mutex::scoped_lock lock(mutex_);

    if(!input || garbageContainer_.find(input) == garbageContainer_.end())
        return;

    GarbageItem& garbageItem = garbageContainer_[input];

    std::get<REF_COUNT>(garbageItem) -= 1;
    if(std::get<REF_COUNT>(garbageItem) <= 0)
    {
        if(strategy == RELEASE_IF_PROCESSED)
        {
            delete std::get<COND_VAR>(garbageContainer_[input]);
            delete std::get<MUTEX>(garbageContainer_[input]);

            garbageContainer_.erase(input);
            delete input;
        }
        else
        {
            std::unique_lock<tbb::mutex> uniqueLock(*std::get<MUTEX>(garbageItem));
            std::get<PRESENT>(garbageItem) = true;
            std::get<COND_VAR>(garbageItem)->notify_one();
        }
    }
}


GarbageCollector::GarbageItem* GarbageCollector::GetGarbageItem(Message* input)
{
	tbb::mutex::scoped_lock lock(mutex_);

	if(!input || garbageContainer_.find(input) == garbageContainer_.end())
		return NULL;

	return &garbageContainer_[input];
}


void GarbageCollector::EraseEntry(Message* input)
{
	tbb::mutex::scoped_lock lock(mutex_);
	if(!input || garbageContainer_.find(input) == garbageContainer_.end())
		return;

	delete std::get<COND_VAR>(garbageContainer_[input]);
	delete std::get<MUTEX>(garbageContainer_[input]);

	garbageContainer_.erase(input);
    delete input;
}
