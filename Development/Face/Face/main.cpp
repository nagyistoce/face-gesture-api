#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>

#include "FaceController.h"
#include "ExceptionDescriptor.h"
#include "GlobalSettings.h"
#include "Tracer.h"

using namespace std;
using namespace cv;
using namespace face;


int main(int argc, char** argv)
{
    int result = 0;
	/* The function can be used to dynamically turn on and off optimized code (code that uses SSE2, AVX, and other
	   instructions on the platforms that support it). It sets a global flag that is further checked by OpenCV functions. */
	if(useOptimized() == false)
		setUseOptimized(true);

	/* The function sets the number of threads used by OpenCV in parallel OpenMP regions. */
	if(getNumThreads() < 100)
		setNumThreads(100);

    try
    {
        GlobalSettingsPtr->SetLocalSettingsFilePath("../LocalSettings.txt"); //HINT Forced initialization.
        GlobalSettingsPtr->SetProcessXml(argc >= 2 ? argv[1] : "Process.default.xml");

        FaceController faceController;
        faceController.LoadFromProcessXml();
        faceController.ExecuteAlgorithmList();
        faceController.ExecuteModuleGraph();
    } 
    catch(ExceptionDescriptor& e)
    {
        e.TraceError();
        result = -1;
    }
    catch(...)
    {
        cerr << "Unknown exception!" << endl;
        result = -1;
    }


    if(GlobalSettingsPtr->IsFlagSet("PauseOnEnd") || result != 0)
        system("pause");

    return result;
}
