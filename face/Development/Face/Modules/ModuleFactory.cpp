#include "ModuleFactory.h"
#include "Tracer.h"

#include "Node.h"
#include "SourceNode.h"
#include "HaarDetectorNode.h"
#include "SourceLimiterNode.h"

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

//typedef multifunction_node<int, std::tuple<continue_msg, continue_msg> > multi_node;
//
//struct MultiBody 
//{
//  void operator()(const int &i, multi_node::output_ports_type &op) 
//  {
//    if(i % 2) 
//      std::get<1>(op).try_put(continue_msg()); // put to odd queue
//    else 
//      std::get<0>(op).try_put(continue_msg()); // put to even queue
//  }
//};

void ModuleFactory::CreateGraph(const VertexContainer& modules)
{
    
  //graph g;

  //queue_node<continue_msg> even_queue(g);
  //queue_node<continue_msg> odd_queue(g);

  //multi_node node1(g, serial, MultiBody());

  //output_port<0>(node1).register_successor(even_queue);
  //make_edge(output_port<1>(node1), odd_queue);

  //for(int i = 0; i < 1000; ++i) {
  //  node1.try_put(i);
  //}
  //g.wait_for_all();



    for(VertexContainer::const_iterator it = modules.begin(); it != modules.end(); it++)
    {
        const string& moduleName = it->first.GetModuleName();
        Node* node = NULL;

        if(moduleName == "Source")
            node = new SourceNode(*it);
        else if(moduleName == "Detector")
            node = new HaarDetectorNode(*it);
        else if(moduleName == "SourceLimiter")
            node = new SourceLimiterNode(*it);
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
