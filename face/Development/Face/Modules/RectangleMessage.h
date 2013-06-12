#pragma once

#include <opencv2/core/core.hpp>
#include "Message.h"
#include "MetaData.h"

namespace face 
{

class RectangleMessage
:   public Message
{
public:
    RectangleMessage(void);
    RectangleMessage(const std::vector<cv::Rect>& rectangles);
    RectangleMessage(const RectangleMessage& other);
    ~RectangleMessage(void);

    RectangleMessage& operator= (RectangleMessage other);

    const std::vector<cv::Rect>& GetRectangles(void) const;

private:
    friend void swap(RectangleMessage& first, RectangleMessage& second);

    std::vector<cv::Rect> rectangles_;
};

}
