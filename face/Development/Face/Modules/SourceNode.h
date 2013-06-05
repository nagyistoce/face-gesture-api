#pragma once

#include <tbb/flow_graph.h>

#include "Node.h"
#include "FaceDef.h"
#include "PortNameParser.h"


class SourceBody;

class SourceNode
:   public Node
{
public:
	SourceNode(const VertexElement& vertexElement);
	virtual ~SourceNode(void);

    SourceNodeType* GetSourceNode(void);
    LimiterNodeType* GetLimiterNode(void);


private:
    virtual void DefinePorts(void);
    virtual void BuildNode(const VertexContainer& modules);
    virtual void CreateEdge(void);
    virtual void RunNode(void);

	SourceBody* sourceBody_;
	SourceNodeType* sourceNode_;
	LimiterNodeType* limiterNode_;
};
