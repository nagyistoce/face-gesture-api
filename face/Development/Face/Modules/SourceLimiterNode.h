#pragma once

#include <tbb/flow_graph.h>

#include "Node.h"
#include "FaceDef.h"
#include "PortNameParser.h"


class SourceLimiterBody;

class SourceLimiterNode
:   public Node
{
public:
    SourceLimiterNode(const VertexElement& vertexElement);
    virtual ~SourceLimiterNode(void);

    MultiNodeContinueType* GetMultiNodeContinue(void);

private:
    virtual void DefinePorts(void);
    virtual void BuildNode(const VertexContainer& modules);
    virtual void CreateEdge(void);

    SourceLimiterBody* sourceLimiterBody_;
    MultiNodeContinueType* multiNodeContinue_;
};
