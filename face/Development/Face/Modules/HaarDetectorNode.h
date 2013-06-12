#pragma once

#include <tbb/flow_graph.h>

#include "Node.h"
#include "FaceDef.h"
#include "PortNameParser.h"

namespace face 
{

class HaarDetectorBody;

class HaarDetectorNode
:   public Node
{
public:
    HaarDetectorNode(const VertexElement& vertexElement);
    virtual ~HaarDetectorNode(void);

    FunctionNode2Type* GetFunction2Node(void);

private:
    virtual void DefinePorts(void);
    virtual void BuildNode(const VertexContainer& modules);
    virtual void CreateEdge(void);

    HaarDetectorBody* haarDetectorBody_;
    FunctionNode2Type* functionNode2_;
    JoinNode2Type* joinNode2_;
};

}
