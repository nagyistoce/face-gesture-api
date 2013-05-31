#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <tbb/flow_graph.h>

#include "Module.h"
#include "FaceDef.h"


class Source
:   public Module
{
public:
    typedef Message* OutputType;
    typedef tbb::flow::source_node<OutputType> SourceNodeType;

    Source(tbb::flow::graph& graph, const std::string& moduleName, const std::string& instanceName);
    virtual ~Source(void);

    bool operator() (OutputType &output);
    SourceNodeType* GetNode(void);

private:
    virtual void BeforeProcess(void);
    virtual void Process(void);
    virtual void AfterProcess(void);

    int frameCounter_;
    cv::VideoCapture videoCapture_;
    cv::Mat frame_;
    OutputType output_;
    SourceNodeType* sourceNode_;
};
