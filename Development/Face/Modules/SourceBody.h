#pragma once

#include <opencv2/highgui/highgui.hpp>

#include "Body.h"
#include "FaceDef.h"
#include "PortNameParser.h"

namespace face 
{

class MetaData;
class SourceParam;

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
	virtual void Process(void);

	cv::VideoCapture videoCapture_;
	MetaData* metaData_;
	SourceParam* param_;
	bool mediaEnd_;
};

}
