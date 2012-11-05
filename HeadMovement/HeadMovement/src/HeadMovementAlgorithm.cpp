#include "Definitions.h"

#include "HeadMovementAlgorithm.h"
#include "LocalSettings.h"
#include "Visualizer.h"

#include "HaarDetector.h"
#include "MotionHistory.h"
#include "PointTracker.h"

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
    pointTracker_(NULL),
    cameraId_(0),
	fileName_(""),
    resolution_(Size(640, 480))
{
	LoadSettingsFromFileStorage();
}

HeadMovementAlgorithm::~HeadMovementAlgorithm(void)
{
	for(ThreadPool::iterator elem = threadPool_.begin(); elem != threadPool_.end(); elem++)
		delete elem->second;

    delete motionHistory_;
    delete pointTracker_;
}

void HeadMovementAlgorithm::LoadSettingsFromFileStorage(void)
{
    FileStorage fileStorage(LocalSettingsPtr->GetProcessXmlFileName(), FileStorage::READ, "UTF-8");
    if(!fileStorage.isOpened())
        CV_Error(1, "Process XML does not exist (" + LocalSettingsPtr->GetProcessXmlFileName() + ")!");

    FileNode node = fileStorage["settings"];
	if(node[0]["media"].isInt())
	{
		node[0]["media"] >> cameraId_;
		videoCapture_.open(cameraId_);
	}
	else if(node[0]["media"].isString())
	{
		node[0]["media"] >> fileName_;
		videoCapture_.open(LocalSettingsPtr->GetInputDirectory() + fileName_);
	}

	if(!videoCapture_.isOpened())
		CV_Error(1, "VideoCapture is not opened!");
    
    node[0]["width"] >> resolution_.width;
    node[0]["height"] >> resolution_.height;

    // Loading motion settings
    node = fileStorage["motion"];

    string pointTracker;
    int bufferSize, mhiDuration, diffThreshold;
    double maxTimeDelta, minTimeDelta;

    node[0]["pointTracker"] >> pointTracker;
    node[0]["bufferSize"] >> bufferSize;
    node[0]["mhiDuration"] >> mhiDuration;
    node[0]["maxTimeDelta"] >> maxTimeDelta;
    node[0]["minTimeDelta"] >> minTimeDelta;
    node[0]["diffThreshold"] >> diffThreshold;
    
    motionHistory_ = new MotionHistory(resolution_, bufferSize, mhiDuration, maxTimeDelta, minTimeDelta, diffThreshold);
    pointTracker_ = new PointTracker(pointTracker);
    // Loading motion settings



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
    // Loading HAAR detectors



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
    // Loading feature extractors
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

		if(frame_.empty())
			break;

        resize(frame_, frame_, resolution_);
        flip(frame_, frame_, 1);

		motionHistory_->UpdateMotionHistory(frame_);

        StartFeatureExtractors();
        VisualizeProcesses();

        faces_ = haarDetectorPool_["FACE"]->GetObjects();
        
        if(!faces_.empty())
        {
            Mat faceMask(frame_.size(), CV_8UC1, Scalar(0));
            rectangle(faceMask, faces_[0], Scalar(255), -1);
            keyPoints_.clear();

            for(LocalFeaturePool::iterator elem = localFeaturePool_.begin(); elem != localFeaturePool_.end(); elem++)
            {
                const vector<KeyPoint>& keyPoints = elem->second->keyPoints;
                for(size_t i = 0; i < keyPoints.size(); i++)
                    keyPoints_.push_back(keyPoints[i]);
            }

            //KeyPointsFilter::runByPixelsMask(keyPoints_, faceMask);
            //KeyPointsFilter::removeDuplicated(keyPoints_);
            if(!keyPoints_.empty() && !prevFrame_.empty())
                pointTracker_->Process(frame_, prevFrame_, faces_[0], keyPoints_);

			Scalar s = mean(motionHistory_->GetMask(), faceMask);
			cout << s[0] << endl;
        }
		
		// press ESC to exit
		if(waitKey(5) >= 0) 
			break;

        prevFrame_ = frame_.clone();

#ifdef DEBUG_MODE
        t = (double)cvGetTickCount() - t;
        double ms = t / ((double)cvGetTickFrequency() * 1000.0);
        double fps = 1000.0 / ms;
        printf("Processing time: %6.2lf ms - %6.2lf fps.\n", ms, fps);
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
        HaarDetector* haarDetector = dynamic_cast<HaarDetector*>(elem->second);

        if(haarDetector)
        {
            haarDetector->SetFrame(frame_);
            haarDetector->Start();
        }

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

	motionHistory_->Visualize();
}
