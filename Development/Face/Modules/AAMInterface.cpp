#include "dolslr/doldef.h"
#include "dolslr/AAMInterface.h"

#include "dolslr/HaarDetector.h"
#include "dolslr/GlobalConfig.h"
#include "dolslr/ExceptionDescriptor.h"

using namespace std;
using namespace cv;

namespace dolslr { 

AAMInterface::AAMInterface(const string& type, const FileNode& fileNode)
:	type_(type),
	boundingBox_(Rect()),
	prevBoundingBox_(Rect()),
	w_(Point2d(0.5, 0.5)),
	shift_(Point2d(0.0, 0.0)),
	shrink_(Point2d(1.0, 1.0))
{
	detector_ = new HaarDetector(type);
	detector_->LoadSettings(fileNode);

	FileNode nodeAamModelFileName	= fileNode["aamModelFileName"];
	FileNode nodeHeightShift		= fileNode["heightShift"];
	FileNode nodeWidthShift			= fileNode["widthShift"];
	FileNode nodeVerticalShrink		= fileNode["verticalShrink"];
	FileNode nodeHorizontalShrink	= fileNode["horizontalShrink"];

	if(!nodeAamModelFileName.isNone())
	{
		aamModelFileName_ = GlobalConfigPtr->GetDirectories().deformable + string(nodeAamModelFileName);
		LoadModel();
	}
	else
	{
		throw ExceptionError("AAM model filename is not defined!");
	}

	if(!nodeHeightShift.isNone() && !nodeWidthShift.isNone())
		shift_ = Point2d(double(nodeWidthShift), double(nodeHeightShift));

	if(!nodeHorizontalShrink.isNone() && !nodeVerticalShrink.isNone())
		shrink_ = Point2d(double(nodeHorizontalShrink), double(nodeVerticalShrink));
}

AAMInterface::~AAMInterface(void)
{
	delete combinedModel;
	delete detector_;
}

void AAMInterface::LoadModel()
{
	ifstream fs(aamModelFileName_.c_str());
	
	if(fs == 0)
		throw ExceptionError("Warning: AAM model file (" + aamModelFileName_ + ") does not exist!");

	int modelType;
	fs >> modelType;

	if(modelType == 0)
	{
		combinedModel = new AAM_Basic(type_);
	}
	else if(modelType == 1)
	{
		combinedModel = new AAM_IC(type_);
	}
	else
	{
		stringstream ss;
		ss << modelType;
		throw ExceptionError("Wrong AAM type: (" + ss.str() + ") does not exist!");
	}

	//read model from file
	combinedModel->Read(fs, aamModelFileName_);
	fs.close();
}

void AAMInterface::Process(Mat& frame, Rect& rect, int drawStyle)
{
	boundingBox_ = Rect();

	Detect(frame, rect);

	if(boundingBox_ != Rect())
	{
		if(prevBoundingBox_ != Rect())
			SmoothBoundingBox();

		Fit(frame, drawStyle);

		prevBoundingBox_ = boundingBox_;
	}
}

void AAMInterface::Detect(Mat& frame, Rect& rect)
{
	vector<Rect> objects;

	detector_->Detect(rect == Rect() ? frame : frame(rect), objects);

	for(int i = 0; i < objects.size(); i++)
		if(objects[i].area() > boundingBox_.area())
			boundingBox_ = objects[i];

	if(boundingBox_ != Rect())
	{
		boundingBox_.x += rect.x;
		boundingBox_.y += rect.y;
	}
}

void AAMInterface::Fit(Mat& frame, int drawStyle)
{
	currentShape = InitShape(currentShape.NPoints() ? currentShape : combinedModel->GetMeanShape(), boundingBox_);

	IplImage* iplFrame = new IplImage(frame);
	combinedModel->Fit(iplFrame, currentShape, 30, false); // TODO: Valahol szivárog egy kis memória - nem mindig
	combinedModel->Draw(iplFrame, drawStyle);
	delete iplFrame;

	// Get imagepoints
	{
		vector<CvPoint2D32f> cvImagePoints = currentShape.getPoints();

		imagePoints.clear();
		for(int i = 0; i < cvImagePoints.size(); i++)
			imagePoints.push_back(Point2d(cvImagePoints[i].x, cvImagePoints[i].y));
	}
}

AAM_Shape AAMInterface::InitShape(const AAM_Shape& pMeanShape, cv::Rect& rect)
{
	AAM_Shape detShape;
	AAM_Shape startShape;

	detShape.resize(2);
	detShape[0].x = rect.x;
	detShape[0].y = rect.y;
	detShape[1].x = detShape[0].x + rect.width;
	detShape[1].y = detShape[0].y + rect.height;

	AdjustShape(detShape);
	AlignShape(startShape, detShape, pMeanShape);

	return startShape;
}

void AAMInterface::AlignShape(AAM_Shape& pStartShape, const AAM_Shape& pDetShape, const AAM_Shape& pMeanShape)
{
	AAM_Shape baseShape, alignedShape;
	baseShape.resize(2);
	alignedShape.resize(2);

	double meanCenter = (pMeanShape.MinY() + pMeanShape.MaxY()) / 2;
	baseShape[0].x = pMeanShape.MinX();
	baseShape[0].y = meanCenter;
	baseShape[1].x = pMeanShape.MaxX();
	baseShape[1].y = meanCenter;

	double yMean = (pDetShape[1].y + pDetShape[0].y)/2;
	alignedShape[0].x = pDetShape[0].x;
	alignedShape[0].y = yMean;
	alignedShape[1].x = pDetShape[1].x;
	alignedShape[1].y = yMean;

	double a, b, tx, ty;
	baseShape.AlignTransformation(alignedShape, a, b, tx, ty);
	pStartShape = pMeanShape;
	pStartShape.TransformPose(a, b, tx, ty);
}

void AAMInterface::AdjustShape(AAM_Shape& pShape)
{
	// following are for aligning base shape to (Viola Jones or other) detector shape
	//static const double heightShift	 = 0.1;  // shift height down by 15%
	//static const double vShrink      = 0.8;	// shrink vertical the VJ box by 30%
	//static const double hShrink      = 1.0;		// shrink horizontal the VJ box by 30%

	double xMin = pShape[0].x;
	double yMin = pShape[0].y;
	double xMax = pShape[1].x;
	double yMax = pShape[1].y;

	double NewHeight = shrink_.y * (yMax - yMin);
	double NewWidth = shrink_.x * (xMax - xMin);
	double yMean = (yMin + yMax) / 2;
	double xMean = (xMin + xMax) / 2;

	yMean += shift_.y * (yMax - yMin);    // move face down
	xMean += shift_.x * (xMax - xMin);    // move face down

	pShape[0].x = xMean - 0.5 * NewWidth;
	pShape[0].y = yMean - 0.5 * NewHeight;
	pShape[1].x = xMean + 0.5 * NewWidth;
	pShape[1].y = yMean + 0.5 * NewHeight;
}

void AAMInterface::SmoothBoundingBox(void)
{
	Rect smoothedBoundingBox;

	smoothedBoundingBox.x = w_.x * prevBoundingBox_.x + w_.y * boundingBox_.x;
	smoothedBoundingBox.y = w_.x * prevBoundingBox_.y + w_.y * boundingBox_.y;
	smoothedBoundingBox.width = w_.x * prevBoundingBox_.width + w_.y * boundingBox_.width;
	smoothedBoundingBox.height = w_.x * prevBoundingBox_.height + w_.y * boundingBox_.height;

	boundingBox_ = smoothedBoundingBox;
}

Rect AAMInterface::BoundingBox(void) const
{
	return boundingBox_;
}

}
