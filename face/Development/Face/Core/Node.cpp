#include <opencv2/core/core.hpp>
#include "Node.h"
#include "Tracer.h"

using namespace std;


tbb::flow::graph Node::graph;
Node::NodeMap Node::nodeMap;


void Node::Run(void)
{
    for(Node::NodeMap::iterator itNode = Node::nodeMap.begin(); itNode != Node::nodeMap.end(); itNode++)
    {
        itNode->second->RunNode();
    }

    graph.wait_for_all();
}


void Node::PushNode(Node* node)
{
    ASSERT(node);
    nodeMap[node->GetFullName()] = node;
}


void Node::CreateGraph(const VertexContainer& modules)
{
    for(Node::NodeMap::iterator itNode = Node::nodeMap.begin(); itNode != Node::nodeMap.end(); itNode++)
        itNode->second->BuildNode(modules);

    for(Node::NodeMap::iterator itNode = Node::nodeMap.begin(); itNode != Node::nodeMap.end(); itNode++)
        itNode->second->CreateEdge();
}


Node::Node(const VertexElement& vertexElement)
:	moduleName_(vertexElement.first.GetModuleName()),
    instanceName_(vertexElement.first.GetInstanceName())
{
    fullName_ = ((instanceName_ == "") ? moduleName_ : (moduleName_ + '.' + instanceName_));
}


Node::~Node(void)
{
}


void Node::RunNode(void)
{

}


void Node::CheckPorts(void)
{
    for(NodeMap::const_iterator itPred = predecessorMap_.begin(); itPred != predecessorMap_.end(); itPred++)
    {
        bool foundPredPort = false;
        for(PortNameMap::const_iterator itPnm = inputPortNameMap_.begin(); itPnm != inputPortNameMap_.end(); itPnm++)
        {
            if(itPnm->second.empty())
                continue;

            if(itPred->first == itPnm->second)
            {
                foundPredPort = true;
                break;
            }
        }

        if(!foundPredPort)
            CV_Error(-1, "Error: Use of undefined port in process.xml: " + GetFullName() + ":" + itPred->first + ".");
    }

    for(PortNameMap::const_iterator itPnm = inputPortNameMap_.begin(); itPnm != inputPortNameMap_.end(); itPnm++)
    {
        bool foundPort = false;
        for(NodeMap::const_iterator itPred = predecessorMap_.begin(); itPred != predecessorMap_.end(); itPred++)
        {
            if(itPred->first == itPnm->second)
            {
                foundPort = true;
                break;
            }
        }

        if(!foundPort)
            CV_Error(-1, "Error: Missing port connection in process.xml: " + GetFullName() + ":" + itPnm->second + ".");
    }    
}


void Node::CollectPredecessors(const VertexContainer& modules)
{
    for(VertexContainer::const_iterator itModul = modules.begin(); itModul != modules.end(); itModul++)
    {
        for(ChildrenList::const_iterator itChild = itModul->second.begin(); itChild != itModul->second.end(); itChild++)
        {
            if(itChild->GetFullName() == GetFullName()) 
                predecessorMap_[itChild->GetPort()] = nodeMap[itModul->first.GetFullName()];
        }
    }
}


const string& Node::GetFullName(void) const
{
    return fullName_;
}


const string& Node::GetModuleName(void) const
{
    return moduleName_;
}


const string& Node::GetInstanceName(void) const
{
    return instanceName_;
}
