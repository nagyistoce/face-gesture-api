#include "Source.h"
#include "Tracer.h"

using namespace std;
using namespace cv;

Source::Source(const string& moduleName, const string& instanceName)
:   Module(moduleName, instanceName),
frameCounter_(0)
{
    videoCapture_.open(0);
    if(videoCapture_.isOpened() == false)
    {
        CV_Error(-1, "Could not opened video capture 0.");
    }
}


Source::~Source(void)
{
}


bool Source::operator() (Message* input)
{
    videoCapture_ >> frame_;

    IMSHOW("+++", frame_);

    frameCounter_++;
    return true;
}
