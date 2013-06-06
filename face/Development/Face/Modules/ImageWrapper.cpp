#include <algorithm>
#include "ImageWrapper.h"

using namespace cv;


void swap(ImageWrapper& first, ImageWrapper& second)
{
    using std::swap;
    swap(first.metaData_, second.metaData_);
    swap(first.rgb_, second.rgb_);
}


ImageWrapper::ImageWrapper(void)
{
}


ImageWrapper::ImageWrapper(const Mat& frame, const MetaData& metaData)
{
    metaData_ = metaData;
    frame.copyTo(rgb_);
}


ImageWrapper::ImageWrapper(const ImageWrapper& other)
{
    metaData_ = other.metaData_;
    other.rgb_.copyTo(rgb_);
}


ImageWrapper::~ImageWrapper(void)
{
}


const Mat& ImageWrapper::Rgb(void) const
{
    return rgb_;
}


const MetaData& ImageWrapper::GetMetaData(void) const
{
    return metaData_;
}


ImageWrapper& ImageWrapper::operator=(ImageWrapper other)
{
    swap(*this, other);
    return *this;
}
