#include "ModuleFactory.h"
#include "Tracer.h"
#include "TbbNode.h"

#include "Message.h"
#include "ImageWrapper.h"

#include "Source.h"
#include "HaarDetector.h"

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
    //for(ModuleMap::iterator it = moduleMap_.begin(); it != moduleMap_.end(); it++)
    //    delete it->second;
}

//struct square {
//    int operator()(int v) { return v*v; }
//};
//
//struct cube {
//    int operator()(int v) { return v*v*v; }
//};
//
//class sum {
//    int &my_sum;
//public:
//    sum( int &s ) : my_sum(s) {}
//    int operator()( std::tuple< int, int > v ) {
//        my_sum += std::get<0>(v) + std::get<1>(v);
//        return my_sum;
//    }
//};

//struct square 
//{
//    Message* operator()(Message* v) 
//    {
//        ImageWrapper* frameIn = dynamic_cast<ImageWrapper*>(v);
//        cv::Mat frameInMat = frameIn->Rgb();
//
//        
//        for(int i = 0; i < 10000000; i++)
//            sqrt(sqrt(sqrt(double(i)))) * sqrt(sqrt(sqrt(double(i)))) * sqrt(sqrt(sqrt(double(i))));
//
//        IMSHOW("square", frameInMat);
//        TRACE("square: " + frameIn->GetMetaData().GetFrameNumber());
//        //Sleep(5000);
//        return v; 
//    }
//};


void ModuleFactory::CreateGraph(const ConnectionMap& modules)
{
    //int result = 0;
    //graph g;
    //broadcast_node<int> input(g);
    //function_node<int,int> squarer( g, unlimited, square() );
    //function_node<int,int> cuber( g, unlimited, cube() );


    //make_edge( input, squarer );
    //make_edge( input, cuber );

    CreateModules(modules);
    CreateConnections(modules);
}


void ModuleFactory::CreateModules(const ConnectionMap& modules)
{
    for(ConnectionMap::const_iterator it = modules.begin(); it != modules.end(); it++)
    {
        const string& moduleName = it->first.GetModuleName();

        if(moduleName == "Source")
        {
            Source* module = new Source(*it);
            TbbNodePtr->PushSource(it->first.GetFullName(), module);
            moduleMap_[it->first] = module;
        }
        else if(moduleName == "Detector")
        {
            HaarDetector* module = new HaarDetector(*it);
            TbbNodePtr->PushHaarDetector(it->first.GetFullName(), module);
            moduleMap_[it->first] = module;
        }
        else
        {
            CV_Error(-1, "Undefined module name in process.xml: " + moduleName + ".");
        }
    }
}

//https://repo.anl-external.org/repos/BlueTBB/tbb40_258oss/examples/graph/dining_philosophers/src/dining_philosophers.cpp
//https://akazarov.web.cern.ch/akazarov/cmt/releases/nightly/tbb/src/test/test_flow_graph.cpp
void ModuleFactory::CreateConnections(const ConnectionMap& modules)
{
    for(ModuleMap::const_iterator it = moduleMap_.begin(); it != moduleMap_.end(); it++)
    {
        Module::PredecessorMap predecessorMap;
        CollectPredecessors(modules, it->first, &predecessorMap);

        if(it->first.GetFullName() == "Detector.face")
            predecessorMap["rectangle"] = NULL;

        it->second->CreateConnection(predecessorMap);
    }
}


void ModuleFactory::CollectPredecessors(const ConnectionMap& modules, const PortNameParser& ppp, Module::PredecessorMap* predecessorMap)
{
    for(ConnectionMap::const_iterator itModul = modules.begin(); itModul != modules.end(); itModul++)
    {
        for(ChildrenList::const_iterator itChild = itModul->second.begin(); itChild != itModul->second.end(); itChild++)
        {
            if(*itChild == ppp) 
            {
                predecessorMap->insert(pair<string, Module*>(itChild->GetPort(), moduleMap_[itModul->first]));
            }
        }
    }
}


void ModuleFactory::Start(void)
{
    for(ModuleMap::const_iterator it = moduleMap_.begin(); it != moduleMap_.end(); it++)
        if(it->first.GetModuleName() == "Source")
        {
           // dynamic_cast<Source*>(it->second)->Start();
        }

    
    graph_.wait_for_all();
}
