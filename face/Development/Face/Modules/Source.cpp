#include "Source.h"
#include "Tracer.h"
#include "ImageWrapper.h"
#include "MetaData.h"

using namespace std;


Source::Source(const ConnectionElement& connectionElement)
:   Module(connectionElement),
    //sourceNode_(NULL),
    metaData_(NULL)
{
    videoCapture_.open(0);
    if(videoCapture_.isOpened() == false)
        CV_Error(-1, "Could not opened video capture 0.");

    if(GetInstanceName() == "front")
        metaData_ = new MetaData(MetaData::POSITION_FRONT, 0, 0);
    else
        metaData_ = new MetaData(MetaData::POSITION_UNDEFINED, 0, 0);

    //limiterNode_ = new LimiterNodeType(graph_, 10, 0);
    //sourceNode_ = new SourceNodeType(graph_, *this, false);

    //tbb::flow::make_edge(*sourceNode_, *limiterNode_);
}


Source::Source(const Source& other)
:   Module(other)
{
    //limiterNode_ = new LimiterNodeType(*other.limiterNode_);
    //sourceNode_ = other.sourceNode_;
    metaData_ = new MetaData(*other.metaData_);
    videoCapture_ = other.videoCapture_;
}


Source::~Source(void)
{
    videoCapture_.release();
    //delete sourceNode_;
    delete metaData_;
}


void Source::DefinePorts(void)
{
    portNameMap_[OUTPUT_DEFAULT] = "";
}


bool Source::operator() (TbbNode::OutputType& output)
{
    BeforeProcess();
    Process();
    AfterProcess();

    output = output_;

    return true;
}


//void Source::Start(void)
//{
//    sourceNode_->activate();
//}


void Source::Process(void)
{
    metaData_->SetTimestamp(GetTimestamp());

    videoCapture_ >> outputFrame_;

    TRACE("Source: " + metaData_->GetFrameNumber());
    IMSHOW(GetFullName(), outputFrame_);

    if(HasOutput())
    {
        output_ = new ImageWrapper(outputFrame_, *metaData_);
    }

    metaData_->IncrementFrameNumber();
}


//Source::LimiterNodeType* Source::GetNode(void)
//{
//    return limiterNode_;
//}
