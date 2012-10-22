#include "Definitions.h"
#include "PointTracker.h"

#include "Visualizer.h"
#include "LocalSettings.h"

using namespace std;
using namespace cv;

PointTracker::PointTracker(string name)
:   name_(name),
    winSize_(Size(21, 21)),
    maxLevel_(3),
    criteriaType_(TermCriteria::COUNT + TermCriteria::EPS),
    criteriaMaxCount_(30),
    criteriaEpsilon_(0.01),
    criteria_(TermCriteria(criteriaType_, criteriaMaxCount_, criteriaEpsilon_)),
    flags_(0), 
    minEigThreshold_(1e-4)
{
    LoadSettingsFromFileStorage();
}


PointTracker::~PointTracker(void)
{
}

void PointTracker::LoadSettingsFromFileStorage(void)
{
    string fileName = LocalSettingsPtr->GetMotionDirectory() + "Settings." + name_ + ".xml";
    FileStorage fileStorage(fileName, FileStorage::READ, "UTF-8");

    if(!fileStorage.isOpened())
        CV_Error(1, "Setting XML does not exist for " + name_ + "!");

    int winSizeWidth, winSizeHeight;
    fileStorage["winSizeWidth"] >> winSizeWidth;
    fileStorage["winSizeHeight"] >> winSizeHeight;
    winSize_ = Size(winSizeWidth, winSizeHeight);

    fileStorage["maxLevel"] >> maxLevel_;
    fileStorage["criteriaMaxCount"] >> criteriaMaxCount_;
    fileStorage["criteriaEpsilon"] >> criteriaEpsilon_;
    fileStorage["flags"] >> flags_;
    fileStorage["minEigThreshold"] >> minEigThreshold_;
}

std::string PointTracker::GetName(void) const
{
    return name_;
}
