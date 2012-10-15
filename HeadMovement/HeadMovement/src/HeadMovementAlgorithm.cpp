#include "HeadMovementAlgorithm.h"

#include "Definitions.h"
#include "HaarDetector.h"
#include "MotionHistory.h"

#include "SiftFeature.h"
#include "SurfFeature.h"
#include "OrbFeature.h"
#include "StarFeature.h"
#include "MserFeature.h"
#include "FastFeature.h"

#include "LbpFeature.h"

#include "LocalSettings.h"

using namespace std;
using namespace cv;

HeadMovementAlgorithm::HeadMovementAlgorithm(void)
:   motionHistory_(NULL),
    cameraId_(0)
{
	LoadSettingsFromFileStorage();

    videoCapture_.open(cameraId_);

    // check if we succeeded
    if(!videoCapture_.isOpened())
        CV_Error(1, "VideoCapture is not opened!");
}

HeadMovementAlgorithm::~HeadMovementAlgorithm(void)
{
    for(GlobalFeaturePool::iterator gfElem = globalFeaturePool_.begin(); gfElem != globalFeaturePool_.end(); gfElem++)
        delete gfElem->second;

    for(LocalFeaturePool::iterator lfElem = localFeaturePool_.begin(); lfElem != localFeaturePool_.end(); lfElem++)
        delete lfElem->second;

    for(HaarDetectorPool::iterator hdElem = haarDetectorPool_.begin(); hdElem != haarDetectorPool_.end(); hdElem++)
        delete hdElem->second;

    delete motionHistory_;
}

void HeadMovementAlgorithm::LoadSettingsFromFileStorage(void)
{
    FileStorage fileStorage(LocalSettingsPtr->GetProcessXmlFileName(), FileStorage::READ, "UTF-8");
    if(!fileStorage.isOpened())
        CV_Error(1, "Process XML does not exist (" + LocalSettingsPtr->GetProcessXmlFileName() + ")!");

    FileNode node = fileStorage["settings"];
    node[0]["cameraId"] >> cameraId_;

    // Loading feature extractors
    node = fileStorage["haarDetectors"];
    for(size_t i = 0; i < node.size(); ++i)
    {
        string name, type;
        node[i]["name"] >> name;

        // Global feature extractors
        if(name.compare("FACE") == 0)
            haarDetectorPool_[name] = new HaarDetector(name);
        else
            CV_Error(1, "Unkonwn HAAR detector (" + name + ")!");
    }


    // Loading feature extractors
    node = fileStorage["featureExtractors"];
    for(size_t i = 0; i < node.size(); ++i)
    {
        string name, type;
        node[i]["name"] >> name;
        node[i]["type"] >> type;

        // Global feature extractors
        if(name.compare("SIFT") == 0)
            globalFeaturePool_[name] = new SiftFeature(name, type);
        else if(name.compare("SURF") == 0)
            globalFeaturePool_[name] = new SurfFeature(name, type);
        else if(name.compare("ORB") == 0)
            globalFeaturePool_[name] = new OrbFeature(name, type);
        else if(name.compare("STAR") == 0)
            globalFeaturePool_[name] = new StarFeature(name, type);
        else if(name.compare("MSER") == 0)
            globalFeaturePool_[name] = new MserFeature(name, type);
        else if(name.compare("FAST") == 0)
            globalFeaturePool_[name] = new FastFeature(name, type);
        else if(name.compare("LBP") == 0)
            localFeaturePool_[name] = new LbpFeature(name, type);
        // TODO: Local feature extractors here:
        // else if(...)
        else
            CV_Error(1, "Unkonwn feature extractor (" + name + ")!");
    }
}

void HeadMovementAlgorithm::Process(void)
{
	cout << "HeadMovementAlgorithm: Press ESC to exit." << endl;

	while(true)
	{
#ifdef DEBUG_MODE
        double t = (double)cvGetTickCount();
#endif

		// get a new frame from camera
		videoCapture_ >> frame_;

        for(HaarDetectorPool::iterator hdElem = haarDetectorPool_.begin(); hdElem != haarDetectorPool_.end(); hdElem++)
        {
            hdElem->second->SetFrame(frame_);
            hdElem->second->Start();
            hdElem->second->Join();
        }

		// detects faces on the incoming frames
		faces_ = haarDetectorPool_["FACE"]->GetObjects();

		if(prevFrame_.empty() == false)
        {
			absdiff(frame_, prevFrame_, subFrame_);
            imshow("HeadMovementAlgorithm - SubFrame", subFrame_);
        }

        if(motionHistory_ == NULL)
            motionHistory_ = new MotionHistory(frame_.size());

        motionHistory_->UpdateMotionHistory(frame_, 30);
        prevFrame_ = frame_.clone();

        DrawObjects(frame_, faces_);

        Mat maskCopy(frame_.rows, frame_.cols, CV_8UC3);
        cvtColor(motionHistory_->GetMask(), maskCopy, CV_GRAY2BGR);
        addWeighted(maskCopy, 0.5, frame_, 1.0, 0.0, frame_);

        imshow("HeadMovementAlgorithm - Frame", frame_);
        imshow("UpdateMotionHistory - MHI", motionHistory_->GetMhi());
        imshow("UpdateMotionHistory - Mask", motionHistory_->GetMask());
        
		// press ESC to exit
		if(waitKey(30) >= 0) 
			break;

#ifdef DEBUG_MODE
        t = (double)cvGetTickCount() - t;
        printf("Processing time: %g ms.\n", t / ((double)cvGetTickFrequency() * 1000.0));
#endif
	}
}

void HeadMovementAlgorithm::DrawObjects(Mat& image, const vector<Rect>& objects)
{
	int i = 0;
	const static Scalar colors[8] =  { 
		CV_RGB(0 ,0, 255),
		CV_RGB(0, 128, 255),
		CV_RGB(0, 255, 255),
		CV_RGB(0, 255, 0),
		CV_RGB(255, 128, 0),
		CV_RGB(255, 255, 0),
		CV_RGB(255, 0, 0),
		CV_RGB(255, 0, 255)
	};

	for(vector<Rect>::const_iterator r = objects.begin(); r != objects.end(); r++, i++)
	{
		Point center(cvRound(r->x + r->width * 0.5), cvRound(r->y + r->height * 0.5));
		int radius = cvRound((r->width + r->height) * 0.25);
		circle(image, center, radius, colors[i % 8], 3);
	}
}
