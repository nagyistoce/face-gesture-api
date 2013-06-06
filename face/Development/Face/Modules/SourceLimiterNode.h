#pragma once

#include <tbb/flow_graph.h>

#include "Node.h"
#include "FaceDef.h"
#include "PortNameParser.h"


class LimitDecrementerBody;

class SourceLimiterNode
:   public Node
{
public:
    SourceLimiterNode(const VertexElement& vertexElement);
    virtual ~SourceLimiterNode(void);

	LimiterNodeType* GetLimiterNode(void);
    MultiNodeContinueType* GetLimitDecrementerNode(void);

private:
    virtual void DefinePorts(void);
    virtual void BuildNode(const VertexContainer& modules);
    virtual void CreateEdge(void);

	LimiterNodeType* limiterNode_;

    LimitDecrementerBody* limitDecrementerBody_;
    MultiNodeContinueType* limitDecrementerNode_;
};
