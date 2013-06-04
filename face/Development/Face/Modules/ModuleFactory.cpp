#include "ModuleFactory.h"
#include "Tracer.h"

#include "Node.h"
#include "SourceNode.h"
#include "HaarDetectorNode.h"

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
}


void ModuleFactory::CreateGraph(const VertexContainer& modules)
{
    for(VertexContainer::const_iterator it = modules.begin(); it != modules.end(); it++)
    {
        const string& moduleName = it->first.GetModuleName();
        Node* node = NULL;

        if(moduleName == "Source")
            node = new SourceNode(*it);
        else if(moduleName == "Detector")
            node = new HaarDetectorNode(*it);
        else
            CV_Error(-1, "Undefined module name in process.xml: " + moduleName + ".");

        Node::PushNode(node);
    }

    Node::CreateGraph(modules);
}


void ModuleFactory::Run(void)
{   
    Node::Run();
}
