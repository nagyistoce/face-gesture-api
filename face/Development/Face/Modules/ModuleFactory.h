#pragma once

#include <string>
#include <tbb/mutex.h>
#include <tbb/flow_graph.h>
#include <opencv2/core/core.hpp>

#include "PortNameParser.h"
#include "FaceDef.h"
#include "Module.h"

#define ModuleFactoryPtr ModuleFactory::GetInstance()


class Message;

class ModuleFactory
{
    typedef std::map<PortNameParser, Module*> ModuleMap;
    //typedef std::tuple<Message*, Message*> Item5;       
    //typedef tbb::flow::function_node<Item5, Message*> FunctionNode;
    //typedef tbb::flow::join_node<Item5, tbb::flow::queueing> JoinNode;

public:
    static ModuleFactory* GetInstance(void);

    void CreateGraph(const ConnectionMap& modules);
    void Start(void);


private:
    static ModuleFactory* instance_;
    static tbb::mutex mutex_;

    ModuleFactory(void);
    ~ModuleFactory(void);

    void CreateModules(const ConnectionMap& modules);
    void CreateConnections(const ConnectionMap& modules);
    void CollectPredecessors(const ConnectionMap& modules, const PortNameParser& ppp, Module::PredecessorMap* predecessors);

    tbb::flow::graph graph_;
    ModuleMap moduleMap_;
};
