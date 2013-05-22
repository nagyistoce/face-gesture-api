#pragma once

#include <string>
#include "tbb/mutex.h"
#include "tbb/flow_graph.h"
#include <opencv2/core/core.hpp>

#include "PortNameParser.h"
#include "FaceDef.h"

#define ModuleFactoryPtr ModuleFactory::GetInstance()

class Message;

class ModuleFactory
{
public:
    static ModuleFactory* GetInstance(void);

    void CreateConnections(const ConnectionMap& sources, const ConnectionMap& modules);
    void Start(void);


private:
    typedef tbb::flow::continue_msg ContinueMsg;
    typedef tbb::flow::broadcast_node<ContinueMsg> BroadcastNode;
    typedef tbb::flow::source_node<Message*> SourceNode;
    typedef tbb::flow::function_node<Message*, Message*> FunctionNode;

    static ModuleFactory* instance_;
    static tbb::mutex mutex_;

    ModuleFactory(void);
    ~ModuleFactory(void);

    tbb::flow::graph graph_;
    std::vector<SourceNode*> sourceNodes_;
    std::vector<FunctionNode*> functionNodes_;
};
