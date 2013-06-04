#pragma once

#include <opencv2/core/core.hpp>
#include "Message.h"
#include "MetaData.h"

class ImageWrapper
:   public Message
{
public:
    ImageWrapper(void);
    ImageWrapper(const cv::Mat& frame, const MetaData& metaData);
    ImageWrapper(const ImageWrapper& other);
    ~ImageWrapper(void);

	//! Const RGB frame getter.
	/*!
		\return Readable RGB frame.
	*/
	const cv::Mat& Rgb(void) const;

    const MetaData& GetMetaData(void) const;

    ImageWrapper& operator=(ImageWrapper other);


private:
    friend void swap(ImageWrapper& first, ImageWrapper& second);

    cv::Mat rgb_;   //!< RGB frame.
    MetaData metaData_;
};
