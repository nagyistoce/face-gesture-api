#pragma once

#include <tbb/flow_graph.h>

#include "Node.h"
#include "FaceDef.h"
#include "PortNameParser.h"

namespace face 
{
    class AAMBody;

    class AAMNode
    :   public Node
    {
    public:
        AAMNode(const VertexElement& vertexElement);
        virtual ~AAMNode(void);

        FunctionNode2Type* GetFunction2Node(void);

    private:
        virtual void DefinePorts(void);
        virtual void BuildNode(const VertexContainer& modules);
        virtual void CreateEdge(void);

        AAMBody* AAMBody_;
        FunctionNode2Type* functionNode2_;
        JoinNode2Type* joinNode2_;
    };
}
