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

    SourceNodeType* SourceNodeNode(void);
    LimiterNodeType* GetLimiterNode(void);


private:
    enum PortNames
    {
        OUTPUT_DEFAULT = 0
    };

    virtual void DefinePorts(void);
    virtual void BuildNode(const VertexContainer& modules);
    virtual void CreateEdge(void);
    virtual void RunNode(void);

	SourceBody* sourceBody_;
	SourceNodeType* sourceNode_;
	LimiterNodeType* limiterNode_;
};
