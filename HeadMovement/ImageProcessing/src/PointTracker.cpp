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
    minEigThreshold_(double(1e-4))
{
    LoadSettingsFromFileStorage();
}


PointTracker::~PointTracker(void)
{
}

void PointTracker::LoadSettingsFromFileStorage(void)
{
    string fileName = LocalSettingsPtr->GetTrackerDirectory() + "Settings." + name_ + ".xml";
    FileStorage fileStorage(fileName, FileStorage::READ, "UTF-8");

    if(!fileStorage.isOpened())
        CV_Error(1, "Setting XML does not exist for " + name_ + "!");

    //string fsUseUniformPatterns;
    //fileStorage["useUniformPatterns"] >> fsUseUniformPatterns;
}

std::string PointTracker::GetName(void) const
{
    return name_;
}
