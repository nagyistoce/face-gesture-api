#include "Definitions.h"

#include "HeadMovementAlgorithm.h"
#include "LocalSettings.h"
#include "Visualizer.h"

#include "HaarDetector.h"
#include "MotionHistory.h"

#include "SiftFeature.h"
#include "SurfFeature.h"
#include "OrbFeature.h"
#include "StarFeature.h"
#include "MserFeature.h"
#include "FastFeature.h"

#include "LbpFeature.h"

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
	for(ThreadPool::iterator elem = threadPool_.begin(); elem != threadPool_.end(); elem++)
		delete elem->second;

    delete motionHistory_;
}

void HeadMovementAlgorithm::LoadSettingsFromFileStorage(void)
{
    FileStorage fileStorage(LocalSettingsPtr->GetProcessXmlFileName(), FileStorage::READ, "UTF-8");
    if(!fileStorage.isOpened())
        CV_Error(1, "Process XML does not exist (" + LocalSettingsPtr->GetProcessXmlFileName() + ")!");

    FileNode node = fileStorage["settings"];
    node[0]["cameraId"] >> cameraId_;

    // Loading HAAR detectors
    node = fileStorage["haarDetectors"];
    for(size_t i = 0; i < node.size(); ++i)
    {
        string name, type;
        node[i]["name"] >> name;

        // HAAR detector
        if(name.compare("FACE") == 0)
            threadPool_[name] = haarDetectorPool_[name] = new HaarDetector(name);
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
            threadPool_[name] = localFeaturePool_[name] = new SiftFeature(name, type);
        else if(name.compare("SURF") == 0)
            threadPool_[name] = localFeaturePool_[name] = new SurfFeature(name, type);
        else if(name.compare("ORB") == 0)
            threadPool_[name] = localFeaturePool_[name] = new OrbFeature(name, type);
        else if(name.compare("STAR") == 0)
            threadPool_[name] = localFeaturePool_[name] = new StarFeature(name, type);
        else if(name.compare("MSER") == 0)
            threadPool_[name] = localFeaturePool_[name] = new MserFeature(name, type);
        else if(name.compare("FAST") == 0)
            threadPool_[name] = localFeaturePool_[name] = new FastFeature(name, type);
        else if(name.compare("LBP") == 0)
            threadPool_[name] = globalFeaturePool_[name] = new LbpFeature(name, type);
        else
            CV_Error(1, "Unkonwn feature extractor (" + name + ")!");
    }
}

void HeadMovementAlgorithm::Process(void)
{
	cout << "HeadMovementAlgorithm: Press ESC to exit." << endl;

	for(HaarDetectorPool::iterator hdElem = haarDetectorPool_.begin(); hdElem != haarDetectorPool_.end(); hdElem++)
	{
		hdElem->second->Start();
		//hdElem->second->Join();
	}

	while(true)
	{
#ifdef DEBUG_MODE
        double t = (double)cvGetTickCount();
#endif
		// get a new frame from camera
		videoCapture_ >> frame_;
        StartDetectors();

		if(motionHistory_ == NULL)
			motionHistory_ = new MotionHistory(frame_.size());

		motionHistory_->UpdateMotionHistory(frame_, 30);

        StartFeatureExtractors();

        VisualizeProcesses();
		
		// press ESC to exit
		if(waitKey(30) >= 0) 
			break;

#ifdef DEBUG_MODE
        t = (double)cvGetTickCount() - t;
        printf("Processing time: %g ms.\n", t / ((double)cvGetTickFrequency() * 1000.0));
#endif
	}
}

void HeadMovementAlgorithm::StartFeatureExtractors(void)
{
    const Mat& motionMask = motionHistory_->GetMask();
    CV_Assert(!motionMask.empty());

    for(ThreadPool::iterator elem = threadPool_.begin(); elem != threadPool_.end(); elem++)
    {
        GlobalFeature* globalFeature = dynamic_cast<GlobalFeature*>(elem->second);
        LocalFeature* localFeature = dynamic_cast<LocalFeature*>(elem->second);

        if(globalFeature)
        {
            globalFeature->SetFrame(motionMask);
            globalFeature->Start();
        }

        if(localFeature)
        {
            localFeature->SetFrame(motionMask);
            localFeature->Start();
        }
    }
}

void HeadMovementAlgorithm::StartDetectors(void)
{
    CV_Assert(!frame_.empty());

    for(HaarDetectorPool::iterator hdElem = haarDetectorPool_.begin(); hdElem != haarDetectorPool_.end(); hdElem++)
    {
        hdElem->second->SetFrame(frame_);
        hdElem->second->Start();
    }
}

void HeadMovementAlgorithm::VisualizeProcesses(void)
{
    for(ThreadPool::iterator elem = threadPool_.begin(); elem != threadPool_.end(); elem++)
    {
        GlobalFeature* globalFeature = dynamic_cast<GlobalFeature*>(elem->second);
        LocalFeature* localFeature = dynamic_cast<LocalFeature*>(elem->second);
        HaarDetector* haarDetector = dynamic_cast<HaarDetector*>(elem->second);

        if(haarDetector)
        {
            haarDetector->Join();
            haarDetector->Visualize();
        }

        if(localFeature)
        {
            localFeature->Join();
            localFeature->Visualize();
        }

        if(globalFeature)
        {
            globalFeature->Join();
            globalFeature->Visualize();
        }
    }
}
