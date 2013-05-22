#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include "Module.h"
#include "FaceDef.h"


class Source
:	public Module
{
public:
    Source(const std::string& moduleName, const std::string& instanceName);
    virtual ~Source(void);

    bool operator() (Message* input);

private:
    int frameCounter_;
    cv::VideoCapture videoCapture_;
    cv::Mat frame_;
};

