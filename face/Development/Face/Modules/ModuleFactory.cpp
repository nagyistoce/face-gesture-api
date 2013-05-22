#include "ModuleFactory.h"
#include "Tracer.h"
#include "Source.h"

using namespace std;
using namespace tbb::flow;

ModuleFactory* ModuleFactory::instance_ = NULL;
tbb::mutex ModuleFactory::mutex_;


ModuleFactory* ModuleFactory::GetInstance(void)
{
    if(instance_ == NULL)
    {
        tbb::mutex::scoped_lock lock(mutex_);
        if(instance_ == NULL)
            instance_ = new ModuleFactory();
    }
    return instance_;
}


ModuleFactory::ModuleFactory(void)
{
}


ModuleFactory::~ModuleFactory(void)
{
    for(size_t i = 0; i < sourceNodes_.size(); i++)
        delete sourceNodes_[i];
}

struct square 
{
    Message* operator()(Message* frameIn) 
    {

        return frameIn; 
    }
};


void ModuleFactory::CreateConnections(const ConnectionMap& sources, const ConnectionMap& modules)
{
    for(size_t i = 0; i < sources.size(); i++)
    {
        const PortNameParser& ppp = sources[i].first;

        SourceNode* A = new SourceNode(graph_, Source(ppp.GetModuleName(), ppp.GetInstanceName()));

        FunctionNode* B = new FunctionNode(graph_, unlimited, square());
        make_edge(*A, *B);
        sourceNodes_.push_back(A);
    }
}


void ModuleFactory::Start(void)
{
    //for(int i = 0; i < 10; ++i) 
    //{
    //    cv::Mat *img = new cv::Mat();
    //    sourceNodes_[0]->try_reserve( img );
    //}

    graph_.wait_for_all();
}
