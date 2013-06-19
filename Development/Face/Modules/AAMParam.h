#pragma once

#include <string>
#include <opencv2/core/core.hpp>

namespace face 
{
    class AAMParam
    {
    public:
        //! Enumerations for choosing drawing mode.
        enum DrawingOptions 
        { 
            DRAW_POINT = 0,			//<! Draw just the shape points
            DRAW_TRIANGLE = 1,		//<! Draw the mesh
            DRAW_APPEARANCE = 2		//<! Draw the full appearance
        };

        AAMParam(const cv::FileStorage& configurationFs);
        AAMParam(const AAMParam& other);
        ~AAMParam(void);

        std::string aamModelFileName;	//!< Path of the AAM model file
        cv::Point2d shift;				//!< Shifting variable for initialize the process
        cv::Point2d shrink;			//!< Shrinking variable for initialize the process
        cv::Rect boundingBox;			//!< Bounding box of the feature
        cv::Rect prevBoundingBox;		//!< Bounding box over the previous frame
        cv::Point2d w;					//!< Weight for smoothing Haar detection
        int drawStyle;                  //!< AAM drawing style
    };
}
