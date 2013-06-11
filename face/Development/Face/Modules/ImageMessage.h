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

    const MetaData& GetMetaData(void) const;

    ImageMessage& operator=(ImageMessage other);


private:
    friend void swap(ImageMessage& first, ImageMessage& second);

    cv::Mat rgb_;   //!< RGB frame.
    MetaData metaData_;
};
