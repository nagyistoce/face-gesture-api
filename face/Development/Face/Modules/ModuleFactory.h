#pragma once

#include <string>
#include <tbb/mutex.h>
#include <tbb/flow_graph.h>
#include <opencv2/core/core.hpp>

#include "PortNameParser.h"
#include "FaceDef.h"

#define ModuleFactoryPtr ModuleFactory::GetInstance()


class Message;
class Source;

class ModuleFactory
{
    //typedef std::tuple<Message*, Message*> Item5;       
    //typedef tbb::flow::function_node<Item5, Message*> FunctionNode;
    //typedef tbb::flow::join_node<Item5, tbb::flow::queueing> JoinNode;

public:
    static ModuleFactory* GetInstance(void);

    void CreateConnections(const ConnectionMap& modules);
    void Start(void);


private:
    static ModuleFactory* instance_;
    static tbb::mutex mutex_;

    ModuleFactory(void);
    ~ModuleFactory(void);

    tbb::flow::graph graph_;
    Source* source_;
};
