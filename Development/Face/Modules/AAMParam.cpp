#include "AAMParam.h"

namespace face 
{
    AAMParam::AAMParam(const cv::FileStorage& configurationFs)
    :	aamModelFileName("01.face.2011.12.11.aam_basic"),
        w(cv::Point2d(0.5, 0.5)),
        shift(cv::Point2d(0.0, 0.0)),
        shrink(cv::Point2d(1.0, 1.0)),
        drawStyle(AAMParam::DRAW_POINT)
    {
        if(!configurationFs["aamModelFileName"].empty() && configurationFs["aamModelFileName"].isString())
        {
            configurationFs["aamModelFileName"] >> aamModelFileName;
        }

        if(!configurationFs["heightShift"].empty() && !configurationFs["widthShift"].empty() && 
            configurationFs["heightShift"].isReal() && configurationFs["widthShift"].isReal())
        {
            double heightShift, widthShift;

            configurationFs["widthShift"] >> widthShift;
            configurationFs["heightShift"] >> heightShift;
            
            shift = cv::Point2d(widthShift, heightShift);
        }

        if(!configurationFs["verticalShrink"].empty() && !configurationFs["horizontalShrink"].empty() && 
            configurationFs["verticalShrink"].isReal() && configurationFs["horizontalShrink"].isReal())
        {
            double verticalShrink, horizontalShrink;

            configurationFs["verticalShrink"] >> verticalShrink;
            configurationFs["horizontalShrink"] >> horizontalShrink;

            shrink = cv::Point2d(verticalShrink, horizontalShrink);
        }

        if(!configurationFs["drawStyle"].empty() && configurationFs["drawStyle"].isString())
        {
            std::string drawStyleStr;
            configurationFs["drawStyle"] >> drawStyleStr;

            if(drawStyleStr == "DRAW_TRIANGLE")
                drawStyle = AAMParam::DRAW_TRIANGLE;
            else if(drawStyleStr == "DRAW_APPEARANCE")
                drawStyle = AAMParam::DRAW_APPEARANCE;
        }
    }


    AAMParam::AAMParam(const AAMParam& other)
    :	aamModelFileName(other.aamModelFileName),
        w(other.w),
        shift(other.shift),
        shrink(other.shrink),
        boundingBox(other.boundingBox),
        prevBoundingBox(other.prevBoundingBox),
        drawStyle(other.drawStyle)
    {
    }


    AAMParam::~AAMParam(void)
    {
    }
}
