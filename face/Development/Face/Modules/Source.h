#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <tbb/flow_graph.h>

#include "Module.h"
#include "FaceDef.h"


class MetaData;

class Source
:   public Module
{
public:
    typedef tbb::flow::source_node<Module::OutputType> SourceNodeType;

    Source(tbb::flow::graph& graph, const std::string& moduleName, const std::string& instanceName);
    virtual ~Source(void);

    bool operator() (Module::OutputType &output);
    SourceNodeType* GetNode(void);

private:
    virtual void Process(void);

    MetaData* metaData_;
    cv::VideoCapture videoCapture_;
    cv::Mat frame_;
    Module::OutputType output_;
    SourceNodeType* sourceNode_;
};
