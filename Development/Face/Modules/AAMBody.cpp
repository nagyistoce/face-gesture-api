#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <fstream>

#include "AAMBody.h"
#include "AAMParam.h"
#include "Tracer.h"
#include "ImageMessage.h"
#include "RectangleMessage.h"
#include "GarbageCollector.h"
#include "GlobalSettings.h"

#include "AAM_Util.h"
#include "AAM_IC.h"
#include "AAM_Basic.h"

using namespace cv;
using namespace std;


namespace face 
{
    AAMBody::AAMBody(const VertexElement& vertexElement)
    :   Body(vertexElement),
        imageMessageIn_(NULL),
        rectangleMessageIn_(NULL),
        param_(NULL),
        combinedModel_(NULL)
    {
        param_ = new AAMParam(GetModulConfigurationFs());
        LoadModel();
    }


    AAMBody::AAMBody(const AAMBody& other)
    :   Body(other),
        imageMessageIn_(NULL),
        rectangleMessageIn_(NULL),
        param_(NULL),
        combinedModel_(other.combinedModel_)
    {
        if(other.param_)
            param_ = new AAMParam(*other.param_);

        if(other.imageMessageIn_)
            imageMessageIn_ = new ImageMessage(*other.imageMessageIn_);
        if(other.rectangleMessageIn_)
            rectangleMessageIn_ = new RectangleMessage(*other.rectangleMessageIn_);

        //LoadModel();
    }


    AAMBody::~AAMBody(void)
    {
        delete imageMessageIn_;
        delete rectangleMessageIn_;
        delete param_;
        //delete combinedModel_;
    }


    void AAMBody::LoadModel(void)
    {
        string modelFileFullPath = GlobalSettingsPtr->GetDirectories().moduleSettings + GetModuleName() + "/" + param_->aamModelFileName;
        ifstream fs(modelFileFullPath.c_str());

        if(fs == 0)
        {
            throw ExceptionError("Error: AAM model file (" + modelFileFullPath + ") does not exist!");
        }

        int modelType;
        fs >> modelType;

        if(modelType == 0)
        {
            combinedModel_ = new AAM_Basic();
        }
        else if(modelType == 1)
        {
            combinedModel_ = new AAM_IC();
        }
        else
        {
            stringstream ss;
            ss << modelType;
            throw ExceptionError("Wrong AAM type: (" + ss.str() + ") does not exist!");
        }

        //read model from file
        combinedModel_->Read(fs, modelFileFullPath);
        fs.close();
    }


    Body::OutputType AAMBody::operator() (Body::InputType2 input)
    {
        imageMessageIn_ = dynamic_cast<ImageMessage*>(std::get<INPUT_IMAGE>(input));
        rectangleMessageIn_ = dynamic_cast<RectangleMessage*>(std::get<INPUT_RECTANGLE>(input));

        TRACE(GetFullName() + ": " + imageMessageIn_->GetMetaData().GetFrameNumber());

        BeforeProcess();
        Process();
        AfterProcess();

        GarbageCollectorPtr->InputHasBeenProcessed(rectangleMessageIn_, GarbageCollector::RELEASE_IF_PROCESSED);
        GarbageCollectorPtr->InputHasBeenProcessed(imageMessageIn_, GarbageCollector::NOTIFY_IF_PROCESSED);

        return output_;
    }


    void AAMBody::operator= (const AAMBody& other)
    {
        WARNING("Equality operator has been disabled.");
        return;
    }


    void AAMBody::Process(void)
    {
        const vector<Rect>& rectangles = rectangleMessageIn_->GetRectangles();
        outputFrame_ = imageMessageIn_->Gray().clone();

        if(rectangles.size() > 0)
        {
            param_->boundingBox = rectangles[0];

            if(param_->prevBoundingBox != Rect())
                SmoothBoundingBox();

            Fit();

            param_->prevBoundingBox = param_->boundingBox;
        }

        IMSHOW(GetFullName(), outputFrame_);
        output_ = HasSuccessor() ? new RectangleMessage() : NULL;
    }


    void AAMBody::SmoothBoundingBox(void)
    {
        Rect smoothedBoundingBox;

        smoothedBoundingBox.x = param_->w.x * param_->prevBoundingBox.x + param_->w.y * param_->boundingBox.x;
        smoothedBoundingBox.y = param_->w.x * param_->prevBoundingBox.y + param_->w.y * param_->boundingBox.y;
        smoothedBoundingBox.width = param_->w.x * param_->prevBoundingBox.width + param_->w.y * param_->boundingBox.width;
        smoothedBoundingBox.height = param_->w.x * param_->prevBoundingBox.height + param_->w.y * param_->boundingBox.height;

        param_->boundingBox = smoothedBoundingBox;
    }


    void AAMBody::Fit(void)
    {
        currentShape_ = InitShape(currentShape_.NPoints() ? currentShape_ : combinedModel_->GetMeanShape());

        IplImage* iplFrame = new IplImage(outputFrame_);
        combinedModel_->Fit(iplFrame, currentShape_, 30, false); // TODO: Valahol szivárog egy kis memória - nem mindig
        combinedModel_->Draw(iplFrame, param_->drawStyle);
        delete iplFrame;

        // Get imagepoints
        {
            vector<CvPoint2D32f> cvImagePoints = currentShape_.getPoints();

            imagePoints_.clear();
            for(int i = 0; i < cvImagePoints.size(); i++)
                imagePoints_.push_back(Point2d(cvImagePoints[i].x, cvImagePoints[i].y));
        }
    }


    AAM_Shape AAMBody::InitShape(const AAM_Shape& pMeanShape)
    {
        AAM_Shape detShape;
        AAM_Shape startShape;

        detShape.resize(2);
        detShape[0].x = param_->boundingBox.x;
        detShape[0].y = param_->boundingBox.y;
        detShape[1].x = detShape[0].x + param_->boundingBox.width;
        detShape[1].y = detShape[0].y + param_->boundingBox.height;

        AdjustShape(detShape);
        AlignShape(startShape, detShape, pMeanShape);

        return startShape;
    }


    void AAMBody::AlignShape(AAM_Shape& pStartShape, const AAM_Shape& pDetShape, const AAM_Shape& pMeanShape)
    {
        AAM_Shape baseShape, alignedShape;
        baseShape.resize(2);
        alignedShape.resize(2);

        double meanCenter = (pMeanShape.MinY() + pMeanShape.MaxY()) * 0.5;
        baseShape[0].x = pMeanShape.MinX();
        baseShape[0].y = meanCenter;
        baseShape[1].x = pMeanShape.MaxX();
        baseShape[1].y = meanCenter;

        double yMean = (pDetShape[1].y + pDetShape[0].y) * 0.5;
        alignedShape[0].x = pDetShape[0].x;
        alignedShape[0].y = yMean;
        alignedShape[1].x = pDetShape[1].x;
        alignedShape[1].y = yMean;

        double a, b, tx, ty;
        baseShape.AlignTransformation(alignedShape, a, b, tx, ty);
        pStartShape = pMeanShape;
        pStartShape.TransformPose(a, b, tx, ty);
    }


    void AAMBody::AdjustShape(AAM_Shape& pShape)
    {
        double xMin = pShape[0].x;
        double yMin = pShape[0].y;
        double xMax = pShape[1].x;
        double yMax = pShape[1].y;

        double NewHeight = param_->shrink.y * (yMax - yMin);
        double NewWidth = param_->shrink.x * (xMax - xMin);
        double yMean = (yMin + yMax) * 0.5;
        double xMean = (xMin + xMax) * 0.5;

        yMean += param_->shift.y * (yMax - yMin);    // move face down
        xMean += param_->shift.x * (xMax - xMin);    // move face down

        pShape[0].x = xMean - 0.5 * NewWidth;
        pShape[0].y = yMean - 0.5 * NewHeight;
        pShape[1].x = xMean + 0.5 * NewWidth;
        pShape[1].y = yMean + 0.5 * NewHeight;
    }
}
