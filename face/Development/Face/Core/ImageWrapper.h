#pragma once

#include <opencv2/core/core.hpp>
#include "Message.h"

class ImageWrapper
:   public Message
{
public:
    enum Position
    {
        POSITION_FRONT = 0,
        POSITION_UNDEFINED
    };

    struct MetaData
    {
        MetaData(void);
        MetaData(const MetaData& other);
        ~MetaData(void);

        Position position;
        int frameNumber;
        unsigned int timestamp;
    };

    ImageWrapper(void);
    ImageWrapper(const ImageWrapper& other);
    ~ImageWrapper(void);

    //! RGB frame getter.
	/*!
		\return Readable/writable RGB frame.
	*/
	cv::Mat& Rgb(void);

	//! Const RGB frame getter.
	/*!
		\return Readable RGB frame.
	*/
	const cv::Mat& Rgb(void) const;

    //! Virtual method from Message.
	/*!
		\sa Message::Clone()

		Creates a new ImageWrapper object from this object.
	*/
	virtual Message* Clone(void) const;

    const MetaData& GetMetaData(void) const;

private:
    cv::Mat rgb_;   //!< RGB frame.
    MetaData metaData_;
};
