#pragma once

#include <map>
#include <tbb/mutex.h>
#include <tbb/flow_graph.h>

#include "FaceDef.h"

#define TbbNodePtr TbbNode::GetInstance()


class Message;
class Source;
class HaarDetector;
class Module;

class TbbNode
{
public:
    typedef Message* OutputType;
    typedef Message* InputType1;

    typedef tbb::flow::source_node<OutputType> SourceNodeType;
    typedef tbb::flow::limiter_node<OutputType> LimiterNodeType;
    typedef tbb::flow::function_node<InputType1, OutputType> FunctionNode1Type;

    typedef std::map<std::string, SourceNodeType*> SourceNodeMap;
    typedef std::map<std::string, LimiterNodeType*> LimiterNodeMap;
    typedef std::map<std::string, FunctionNode1Type*> FunctionNode1Map;

    static TbbNode* GetInstance(void);

    void PushNode(const std::string& fullName, Module* module);
    void PushSource(const std::string& fullName, Source* module);
    void PushHaarDetector(const std::string& fullName, HaarDetector* module);
    void SetGraph(tbb::flow::graph& graph);


private:
    static TbbNode* instance_;
    static tbb::mutex mutex_;

    TbbNode(void);
    ~TbbNode(void);

    tbb::flow::graph* graph_;

    SourceNodeMap sourceNodeMap_;
    LimiterNodeMap limiterNodeMap_;
    FunctionNode1Map functionNode1map_;
};
