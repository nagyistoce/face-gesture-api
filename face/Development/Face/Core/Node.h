#pragma once

#include <map>
#include <string>
#include <tbb/flow_graph.h>

#include "Body.h"
#include "PortNameParser.h"


class Message;

//http://software.intel.com/sites/products/documentation/doclib/tbb_sa/help/reference/flow_graph.htm
class Node
{
public:
    typedef std::map<std::string, Node*> NodeMap;

    typedef tbb::flow::broadcast_node<tbb::flow::continue_msg> BroadcastContinueNodeType;
    typedef tbb::flow::source_node<Body::OutputType> SourceNodeType;
    typedef tbb::flow::limiter_node<Body::OutputType> LimiterNodeType;
    typedef tbb::flow::function_node<Body::InputType1, Body::OutputType> FunctionNode1Type;

    static void Run(void);
    static void PushNode(Node* node);
    static void CreateGraph(const VertexContainer& modules);

    static tbb::flow::graph graph;
    static NodeMap nodeMap;

    //! Full name getter.
    const std::string& GetFullName(void) const;

    //! Module name getter.
    const std::string& GetModuleName(void) const;

    //! Instance name getter.
    const std::string& GetInstanceName(void) const;


protected:
    Node(const VertexElement& vertexElement);
    virtual ~Node(void);

    virtual void BuildNode(const VertexContainer& modules) = 0;
    virtual void CreateEdge(void) = 0;
    virtual void DefinePorts(void) = 0;
    virtual void RunNode(void);

    void CollectPredecessors(const VertexContainer& modules);
    void CheckPorts(void);

    std::string fullName_;          //!< Full name of the module: "ModuleName.instanceName".
    std::string moduleName_;        //!< Module name.
    std::string instanceName_;      //!< Instance name.

    std::map<int, std::string> portNameMap_;
    NodeMap predecessorMap_;
};
