#include "TbbNode.h"
#include "Source.h"
#include "HaarDetector.h"

using namespace std;
using namespace tbb::flow;


TbbNode* TbbNode::instance_ = NULL;
tbb::mutex TbbNode::mutex_;


TbbNode* TbbNode::GetInstance(void)
{
    if(instance_ == NULL)
    {
        tbb::mutex::scoped_lock lock(mutex_);
        if(instance_ == NULL)
            instance_ = new TbbNode();
    }
    return instance_;
}


TbbNode::TbbNode(void)
{
}


TbbNode::~TbbNode(void)
{
}


void PushNode(const std::string& fullName, Module* module)
{

}

void TbbNode::PushSource(const std::string& fullName, Source* module)
{
    sourceNodeMap_[fullName] = new SourceNodeType(*graph_, *module);
    limiterNodeMap_[fullName] = new LimiterNodeType(*graph_, 10, 0);
    make_edge(*sourceNodeMap_[fullName], *limiterNodeMap_[fullName]);
}


void TbbNode::PushHaarDetector(const std::string& fullName, HaarDetector* module)
{
    functionNode1map_[fullName] = new FunctionNode1Type(*graph_, tbb::flow::serial, *module);
}


void TbbNode::SetGraph(tbb::flow::graph& graph)
{
    graph_ = &graph;
}
