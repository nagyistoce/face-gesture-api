#include <iostream>

#include "opencv2/opencv.hpp"
#include "HeadMovementAlgorithm.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	int result = 0;
	HeadMovementAlgorithm headMovementAlgorithm;

	try
	{
		headMovementAlgorithm.ProcessAlgorithm();
	}
	catch(Exception& e)
	{
		cout << "Exception caught: " << e.what() << endl;
		result = -1;
	}

	return result;
}
