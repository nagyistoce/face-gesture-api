#include "ImageWrapper.h"

using namespace cv;

ImageWrapper::MetaData::MetaData(void)
{
    position = POSITION_UNDEFINED;
    frameNumber = 0;
    timestamp = 0;
}


ImageWrapper::MetaData::MetaData(const ImageWrapper::MetaData& other)
{
    position = other.position;
    frameNumber = other.frameNumber;
    timestamp = other.timestamp;
}


ImageWrapper::MetaData::~MetaData(void)
{
}


ImageWrapper::ImageWrapper(void)
{
}


ImageWrapper::ImageWrapper(const ImageWrapper& other)
{
    metaData_ = ImageWrapper::MetaData(other.GetMetaData());

    other.rgb_.copyTo(rgb_);
}


ImageWrapper::~ImageWrapper(void)
{
}


Mat& ImageWrapper::Rgb(void)
{
    return rgb_;
}


const Mat& ImageWrapper::Rgb(void) const
{
    return rgb_;
}


Message* ImageWrapper::Clone(void) const
{
    return new ImageWrapper(*this);
}


const ImageWrapper::MetaData& ImageWrapper::GetMetaData() const
{
    return metaData_;
}