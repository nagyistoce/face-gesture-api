#pragma once

#include <tbb/flow_graph.h>

#include "Node.h"
#include "FaceDef.h"
#include "PortNameParser.h"

namespace face 
{

class SourceBody;

class SourceNode
:   public Node
{
public:
	SourceNode(const VertexElement& vertexElement);
	virtual ~SourceNode(void);

    SourceNodeType* GetSourceNode(void);


private:
    virtual void DefinePorts(void);
    virtual void BuildNode(const VertexContainer& modules);
    virtual void CreateEdge(void);
    virtual void RunNode(void);

	SourceBody* sourceBody_;
	SourceNodeType* sourceNode_;
};

}
