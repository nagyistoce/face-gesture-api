#pragma once

#include <tbb/flow_graph.h>

#include "Node.h"
#include "FaceDef.h"
#include "PortNameParser.h"

namespace face 
{

class EmptyBody;

class EmptyNode
:   public Node
{
public:
    EmptyNode(const VertexElement& vertexElement);
    virtual ~EmptyNode(void);

    FunctionNode1Type* GetFunctionNode(void);

private: 
    virtual void DefinePorts(void); 
    virtual void BuildNode(const VertexContainer& modules);   
    virtual void CreateEdge(void);
 
    EmptyBody* emptyBody_; 
    FunctionNode1Type* functionNode_; 
};

}
