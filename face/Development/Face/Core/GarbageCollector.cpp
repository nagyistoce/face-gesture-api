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
}


void GarbageCollector::ParseConnectionMap(const VertexContainer& modules)
{
    ASSERT(inputMap_.empty());

    for(VertexContainer::const_iterator it = modules.begin(); it != modules.end(); it++)
    {
        inputMap_[it->first.GetFullName()] = it->second.size();
    }
}


void GarbageCollector::PushNewOutput(Message* newItem, const std::string& moduleFullName)
{
    {
        tbb::mutex::scoped_lock lock(mutex_);
        moduleOutputMap_[newItem] = inputMap_[moduleFullName];
    }
}


bool GarbageCollector::InputHasBeenProcessed(Message* input)
{
    {
        tbb::mutex::scoped_lock lock(mutex_);
        moduleOutputMap_[input]--;

        if(moduleOutputMap_[input] <= 0)
        {
            moduleOutputMap_.erase(input);
            return true;
        }
    }

    return false;
}
