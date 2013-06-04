#pragma once
#pragma warning(disable : 4251 4275) /* needs to have dll-interface */

#include <tbb/flow_graph.h>


class Message;

typedef Message* OutputType;
typedef Message* InputType1;

typedef tbb::flow::source_node<OutputType> SourceNodeType;
typedef tbb::flow::limiter_node<OutputType> LimiterNodeType;
typedef tbb::flow::function_node<InputType1, OutputType> FunctionNode1Type;
