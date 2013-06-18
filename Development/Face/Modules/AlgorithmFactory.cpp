#include "AlgorithmFactory.h"
#include "TestAlgorithm.h"
#include "Algorithm.h"
#include "Tracer.h"

using namespace std;
using namespace tbb::flow;

namespace face 
{

AlgorithmFactory* AlgorithmFactory::instance_ = NULL;
tbb::mutex AlgorithmFactory::mutex_;

// http://goparallel.sourceforge.net/complex-graphs-threading-building-blocks/
// http://software.intel.com/en-us/blogs/2011/09/09/a-feature-detection-example-using-the-intel-threading-building-blocks-flow-graph
// http://software.intel.com/en-us/blogs/2011/09/14/how-to-make-a-pipeline-with-an-intel-threading-building-blocks-flow-graph
// http://www.drdobbs.com/tools/the-intel-threading-building-blocks-flow/231900177?pgno=2
// http://threadingbuildingblocks.org/docs/help/reference/flow_graph.htm
AlgorithmFactory::AlgorithmFactory(void)
{
    broadcastNode_ = new BroadcastNode(graph_);
}


AlgorithmFactory* AlgorithmFactory::GetInstance(void)
{
    if(instance_ == NULL)
    {
        tbb::mutex::scoped_lock lock(mutex_);
        if(instance_ == NULL)
            instance_ = new AlgorithmFactory();
    }
    return instance_;
}


void AlgorithmFactory::Push(const string& name, const string& config)
{
	if(name == "test")
    {
        ContinueNode* A = new ContinueNode(graph_, TestAlgorithm(name, config));
        make_edge(*broadcastNode_, *A);
        continueNodes_.push_back(A);
    }
}


void AlgorithmFactory::Run(void)
{
    broadcastNode_->try_put(continue_msg());
    graph_.wait_for_all();
    Release();
}


void AlgorithmFactory::Release(void)
{
    delete broadcastNode_;

    for(int i = 0; i < int(continueNodes_.size()); i++)
        delete continueNodes_[i];
}

}
