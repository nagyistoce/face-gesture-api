#include "GarbageCollector.h"
#include "Tracer.h"

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
        delete std::get<MUTEX>(it->second);
        delete std::get<COND_VAR>(it->second);
    }

    garbageContainer_.clear();
}


void GarbageCollector::ParseConnectionMap(const VertexContainer& modules)
{
    ASSERT(inputMap_.empty());

    for(VertexContainer::const_iterator it = modules.begin(); it != modules.end(); it++)
    {
        inputMap_[it->first.GetFullName()] = it->second.size();
    }
}


void GarbageCollector::PushNewOutput(Message* newOutput, const std::string& moduleFullName)
{
    {
        tbb::mutex::scoped_lock lock(mutex_);

        if(garbageContainer_.find(newOutput) != garbageContainer_.end())
            return;

        garbageContainer_[newOutput] = std::make_tuple(inputMap_[moduleFullName], new tbb::mutex(), new std::condition_variable(), false);
    }
}

// imagewrapper
void GarbageCollector::InputHasBeenProcessed(Message* input)
{
    {
        tbb::mutex::scoped_lock lock(mutex_);

        if(garbageContainer_.find(input) == garbageContainer_.end())
            return;

        GarbageItem& garbageItem = garbageContainer_[input];
        std::get<REF_COUNT>(garbageItem) -= 1;

        if(std::get<REF_COUNT>(garbageItem) <= 0)
        {
            std::unique_lock<tbb::mutex> uniqueLock(*std::get<MUTEX>(garbageItem));
            std::get<PRESENT>(garbageItem) = true;
            std::get<COND_VAR>(garbageItem)->notify_one();

            delete std::get<MUTEX>(garbageItem);
            delete std::get<COND_VAR>(garbageItem);

            garbageContainer_.erase(input);
        }
    }
}

//void consumer() {
//    while( !present ) {
//        unique_lock<tbb::mutex> ul( my_mtx );
//        my_condition.wait( ul );
//    }