#include "Source.h"
#include "Tracer.h"
#include "ImageWrapper.h"
#include "MetaData.h"

using namespace std;


Source::Source(tbb::flow::graph& graph, const string& moduleName, const string& instanceName)
:   Module(moduleName, instanceName),
    sourceNode_(NULL)
{
    videoCapture_.open(0);
    if(videoCapture_.isOpened() == false)
        CV_Error(-1, "Could not opened video capture 0.");

    if(instanceName == "front")
        metaData_ = new MetaData(MetaData::POSITION_FRONT, 0, 0);
    else
        metaData_ = new MetaData(MetaData::POSITION_UNDEFINED, 0, 0);

    sourceNode_ = new SourceNodeType(graph, *this);
}


Source::~Source(void)
{
    videoCapture_.release();
    delete sourceNode_;
    delete metaData_;
}


bool Source::operator() (Module::OutputType &output)
{
    BeforeProcess();
    Process();
    AfterProcess(output_);

    output = output_;

    return true;
}


Source::SourceNodeType* Source::GetNode(void)
{
    return sourceNode_;
}


void Source::Process(void)
{
    metaData_->SetTimestamp(GetTimestamp());

    videoCapture_ >> frame_;
    output_ = new ImageWrapper(frame_, *metaData_);
    TRACE("Source: " + metaData_->GetFrameNumber());
    IMSHOW(GetFullName(), frame_);

    metaData_->IncrementFrameNumber();
}
