//#pragma once
//
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/core/core.hpp>
//#include <tbb/flow_graph.h>
//
//#include "Module.h"
//#include "FaceDef.h"
//#include "PortNameParser.h"
//#include "TbbNode.h"
//
//
//class MetaData;
//
//class Source
//:   public Module
//{
//public:
//    //typedef tbb::flow::source_node<Module::OutputType> SourceNodeType;
//    //typedef tbb::flow::limiter_node<Module::OutputType> LimiterNodeType;
//
//    enum PortNames
//    {
//        OUTPUT_DEFAULT = 0
//    };
//
//    Source(const VertexElement& connectionElement);
//    Source(const Source& other);
//    virtual ~Source(void);
//
//    bool operator() (TbbNode::OutputType& output);
//    Source& operator= (const Source& /*other*/) {
//        return *this;
//    }
//
//    //void Start(void);
//    //LimiterNodeType* GetNode(void);
//
//private:
//    virtual void DefinePorts(void);
//    virtual void Process(void);
//
//    MetaData* metaData_;
//    cv::VideoCapture videoCapture_;
//    //SourceNodeType* sourceNode_;
//    //LimiterNodeType* limiterNode_;
//};
