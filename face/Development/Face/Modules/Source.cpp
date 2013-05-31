#include "Source.h"
#include "Tracer.h"
#include "ImageWrapper.h"

using namespace std;
using namespace cv;


Source::Source(tbb::flow::graph& graph, const string& moduleName, const string& instanceName)
:   Module(moduleName, instanceName),
    sourceNode_(NULL),
    frameCounter_(0)
{
    videoCapture_.open(0);
    if(videoCapture_.isOpened() == false)
    {
        CV_Error(-1, "Could not opened video capture 0.");
    }

    sourceNode_ = new SourceNodeType(graph, *this);
}


Source::~Source(void)
{
    videoCapture_.release();
    delete sourceNode_;
}


bool Source::operator() (OutputType &output)
{
    BeforeProcess();
    Process();
    AfterProcess();

    output = output_;

    return true;
}


Source::SourceNodeType* Source::GetNode(void)
{
    return sourceNode_;
}


void Source::BeforeProcess(void)
{
}


void Source::Process(void)
{
    videoCapture_ >> frame_;
    output_ = new ImageWrapper(frame_, MetaData(MetaData::POSITION_FRONT, frameCounter_, 0L));
    IMSHOW(GetFullName(), frame_);
}


void Source::AfterProcess(void)
{
    frameCounter_++;
    //TRACE(frameCounter_);
}
