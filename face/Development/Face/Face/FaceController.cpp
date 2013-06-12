#include "FaceController.h"

#include <windows.h>
#include <sstream>
#include <iostream>
#include <opencv2/core/core.hpp>

#include "GlobalSettings.h"
#include "Tracer.h"
#include "Configuration.h"
#include "XmlReader.h"
#include "AlgorithmFactory.h"
#include "ModuleFactory.h"


using namespace std;
using namespace cv;


namespace face 
{

FaceController::FaceController(void)
{
}


FaceController::~FaceController(void)
{
}


void FaceController::LoadFromProcessXml(void)
{
    string fileName = GlobalSettingsConstPtr->GetProcessXml();
    LOG("--- '" + fileName + "' ---");
    XmlReader reader(fileName, true);

    LoadDefaultSettings(reader.GetFileStorage());

    BuildAlgorithmList(reader.GetFileStorage());
    BuildModuleGraph(reader.GetFileStorage());
}


void FaceController::LoadDefaultSettings(const FileStorage& fileStorage) 
{
    //bool live = XmlReader::ReadBool(fileStorage.root(), "live", true);
    //SuitePtr->SetMode(live, record);

    GlobalSettingsPtr->LoadFlags(fileStorage["flags"]);
    GlobalSettingsPtr->LoadValues(fileStorage["values"]);

    const FileNode& suiteName = fileStorage["suite"];
    if(suiteName.isNone() == false)
    {
        ASSERT(string(suiteName).empty() == false);
    }
}


void FaceController::BuildAlgorithmList(const FileStorage& fileStorage) 
{
    LOG("::Build AlgorithmList::");
    algorithmList_.BuildFromFileNode(fileStorage["algorithms"]);
}


void FaceController::ExecuteAlgorithmList(void)
{
    LOG("::Start AlgorithmList::");
    AlgorithmFactoryPtr->Run();
    LOG("::End AlgorithmList::");
}


void FaceController::BuildModuleGraph(const FileStorage& fileStorage) 
{
    LOG("::Build ModuleGraph::");

    // Processes.
    const FileNode& tasksNode = fileStorage["tasks"];

    //ModuleGraph::Options options;
    //options.live = XmlReader::ReadBool(fileStorage.root(), "live", true);
    //options.pufferSize = XmlReader::ReadInt(fileStorage.root(), "pufferSize", 10);
    //options.record = XmlReader::ReadBool(fileStorage.root(), "record", false);

    moduleGraph_.BuildFromFileNode(tasksNode);
}


void FaceController::ExecuteModuleGraph(void)
{
    LOG("::Start ModuleGraph::");
    ModuleFactoryPtr->Run();
    LOG("::End ModuleGraph::");
}

}
