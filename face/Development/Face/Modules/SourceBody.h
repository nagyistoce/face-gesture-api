#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include "Body.h"
#include "FaceDef.h"
#include "PortNameParser.h"


class MetaData;

class SourceBody
:   public Body
{
public:
    enum InputPortNames
    {
		NO_INPUT = 0
    };

	enum OutputPortNames
	{
		OUTPUT_DEFAULT = 0
	};

	SourceBody(const VertexElement& vertexElement);
	SourceBody(const SourceBody& other);
	virtual ~SourceBody(void);

	bool operator() (Body::OutputType& output);
	void operator= (const SourceBody& other);


private:
	enum SourceType
	{
		CAMERA = 0,
		VIDEO_FILE
	};

	virtual void Process(void);

	MetaData* metaData_;
	int sourceType_;
	int cameraId_;
	std::string videoFilePath_;
	cv::VideoCapture videoCapture_;
};
