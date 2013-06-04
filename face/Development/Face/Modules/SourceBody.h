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
	enum PortNames
	{
		OUTPUT_DEFAULT = 0
	};

	SourceBody(const ConnectionElement& connectionElement);
	SourceBody(const SourceBody& other);
	virtual ~SourceBody(void);

	bool operator() (OutputType& output);
	void operator= (const SourceBody& other);

	void Start(void);


private:
	enum SourceType
	{
		CAMERA = 0,
		VIDEO_FILE
	};

	virtual void DefinePorts(void);
	virtual void Process(void);

	MetaData* metaData_;
	int sourceType_;
	int cameraId_;
	std::string videoFilePath_;
	cv::VideoCapture videoCapture_;
};
