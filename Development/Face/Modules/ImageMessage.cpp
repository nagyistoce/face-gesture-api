#include <algorithm>
#include <opencv2/imgproc/imgproc.hpp>
#include "ImageMessage.h"

using namespace cv;

namespace face 
{

void swap(ImageMessage& first, ImageMessage& second)
{
    using std::swap;
    swap(first.metaData_, second.metaData_);
    swap(first.rgb_, second.rgb_);
	swap(first.gray_, second.gray_);
	swap(first.normalized_, second.normalized_);
}


ImageMessage::ImageMessage(void)
{
}


ImageMessage::ImageMessage(const Mat& frame, const MetaData& metaData)
{
    metaData_ = metaData;
    frame.copyTo(rgb_);

	cvtColor(rgb_, gray_, CV_BGR2GRAY);
	equalizeHist(gray_, normalized_);
}


ImageMessage::ImageMessage(const ImageMessage& other)
{
    metaData_ = other.metaData_;
    other.rgb_.copyTo(rgb_);
	other.gray_.copyTo(gray_);
	other.normalized_.copyTo(normalized_);
}


ImageMessage::~ImageMessage(void)
{
}


const Mat& ImageMessage::Rgb(void) const
{
    return rgb_;
}


const Mat& ImageMessage::Gray(void) const
{
	return gray_;
}


const Mat& ImageMessage::Normalized(void) const
{
	return normalized_;
}


const MetaData& ImageMessage::GetMetaData(void) const
{
    return metaData_;
}


cv::Size ImageMessage::GetSize(void) const
{
    return rgb_.size();
}


int ImageMessage::GetWidth(void) const
{
	return rgb_.cols;
}


int ImageMessage::GetHeight(void) const
{
	return rgb_.rows;
}


ImageMessage& ImageMessage::operator=(ImageMessage other)
{
    swap(*this, other);
    return *this;
}

}
