#pragma once

#include <opencv2/objdetect/objdetect.hpp>
#include "Body.h"
#include "FaceDef.h"
#include "PortNameParser.h"
#include "AAM_Shape.h"

class AAM;

namespace face 
{
    class AAMParam;
    class ImageMessage;
    class RectangleMessage;

    class AAMBody
    :   public Body
    {
    public:
        enum InputPortNames
        {
            INPUT_IMAGE = 0,
            INPUT_RECTANGLE
        };

        enum OutputPortNames
        {
            OUTPUT_DEFAULT = 0
        };

        AAMBody(const VertexElement& vertexElement);
        AAMBody(const AAMBody& other);
        virtual ~AAMBody(void);

        Body::OutputType operator() (Body::InputType2 input);
        void operator= (const AAMBody& other);


    private:
        virtual void Process(void);

        //! Fitting the model over the current frame
	    /*
		    \param frame Current frame
	    */
	    void Fit(void);

        //! Loading the AAM model file (face-gesture-api\Testing\Input\Modules\AAM\something.aam_basic).
        void LoadModel(void);

        //! Smoothing the result of Haar detector (average filter)
        void SmoothBoundingBox(void);

        //! Detect an object on the image
	    /*
		    \param pMeanShape Mean shape of the feature
	    */
	    AAM_Shape InitShape(const AAM_Shape& pMeanShape);

	    //! Align MeanShape to the bounding box
	    /*
	    	\param StartShape Start-up shape
	    	\param DetShape Determined shape
	    	\param MeanShape Mean shape
    	*/
    	void AlignShape(AAM_Shape& StartShape, const AAM_Shape& DetShape, const AAM_Shape& MeanShape);

        //! Make the ones face box smaller and move it down a bit
    	/*
	    	\param Shape Shape for adjusting
    	*/
	    void AdjustShape(AAM_Shape& Shape);


        AAM* combinedModel_;	//!< The combined AAM model.
        ImageMessage* imageMessageIn_;
        RectangleMessage* rectangleMessageIn_;
        AAMParam* param_;

        std::vector<cv::Point2d> imagePoints_;	//!< The shape points.
        AAM_Shape currentShape_;			//!< Current AAM shape
    };
}
