#include <algorithm>
#include "RectangleMessage.h"

using namespace cv;


void swap(RectangleMessage& first, RectangleMessage& second)
{
    using std::swap;
    swap(first.rectangles_, second.rectangles_);
}


RectangleMessage::RectangleMessage(void)
{
}


RectangleMessage::RectangleMessage(const std::vector<cv::Rect>& rectangles)
{
    rectangles_ = rectangles;
}


RectangleMessage::RectangleMessage(const RectangleMessage& other)
{
    rectangles_ = other.rectangles_;
}


RectangleMessage::~RectangleMessage(void)
{
}


const std::vector<cv::Rect>& RectangleMessage::GetRectangles(void) const
{
    return rectangles_;
}


RectangleMessage& RectangleMessage::operator=(RectangleMessage other)
{
    swap(*this, other);
    return *this;
}
