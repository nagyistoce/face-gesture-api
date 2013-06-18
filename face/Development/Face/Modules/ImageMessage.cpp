#include <algorithm>
#include "ImageMessage.h"

using namespace cv;


void swap(ImageMessage& first, ImageMessage& second)
{
    using std::swap;
    swap(first.metaData_, second.metaData_);
    swap(first.rgb_, second.rgb_);
}


ImageMessage::ImageMessage(void)
{
}


ImageMessage::ImageMessage(const Mat& frame, const MetaData& metaData)
{
    metaData_ = metaData;
    frame.copyTo(rgb_);
}


ImageMessage::ImageMessage(const ImageMessage& other)
{
    metaData_ = other.metaData_;
    other.rgb_.copyTo(rgb_);
}


ImageMessage::~ImageMessage(void)
{
}


const Mat& ImageMessage::Rgb(void) const
{
    return rgb_;
}


const MetaData& ImageMessage::GetMetaData(void) const
{
    return metaData_;
}


ImageMessage& ImageMessage::operator=(ImageMessage other)
{
    swap(*this, other);
    return *this;
}
