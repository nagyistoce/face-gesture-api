#pragma once

#include <opencv2/core/core.hpp>
#include "Message.h"
#include "MetaData.h"

class ImageMessage
:   public Message
{
public:
    ImageMessage(void);
    ImageMessage(const cv::Mat& frame, const MetaData& metaData);
    ImageMessage(const ImageMessage& other);
    ~ImageMessage(void);

	//! Const RGB frame getter.
	/*!
		\return Readable RGB frame.
	*/
	const cv::Mat& Rgb(void) const;
	const cv::Mat& Gray(void) const;
	const cv::Mat& Normalized(void) const;

    const MetaData& GetMetaData(void) const;

	int GetWidth(void) const;
	int GetHeight(void) const;

    ImageMessage& operator=(ImageMessage other);


private:
    friend void swap(ImageMessage& first, ImageMessage& second);

    cv::Mat rgb_;   //!< RGB frame.
	cv::Mat gray_;
	cv::Mat normalized_;
    MetaData metaData_;
};
