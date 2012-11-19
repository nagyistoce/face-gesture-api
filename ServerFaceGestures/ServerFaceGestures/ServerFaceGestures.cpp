#include <iostream>
#include <string>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"

#include "TcpIp.h"

using namespace std;

int main(int argc, char* argv[])
{	
	int key = 0;
	streamServer* server = NULL;

	try
	{
		server = new streamServer(6000);		
		server->start();
		cvNamedWindow("tcp_images", 1);
		cout << "If you type in an 'x', the program ends." << endl;

		while(true) 
		{
			key = cvWaitKey(3);
			if (key == 'x')
				break;
		}
	} 
	catch (string s)
	{
		cout << (string("ERROR: ") + s) << endl;
	}
	
	if (server)
	{
		server->stop();
		delete server;
		cvDestroyWindow("tcp_images");
	}

	return 0;
}
