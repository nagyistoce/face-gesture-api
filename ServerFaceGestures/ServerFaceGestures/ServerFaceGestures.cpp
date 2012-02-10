#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>
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

//#include "iFaceTracker.h"
//#include <string>
//#include <iostream>
//#include <sstream>
//#include <ctime>
//#include <cctype>
//
//void		Help( void );
//CvCapture*	InitCapture( int argc, char** argv );
//IplImage*	CaptureFrame( CvCapture* capture );
//
//int main( int argc, char** argv )
//{
//	Help();
//
//    IFaceTracker* iface = new IFaceTracker( 
//		0 | DETECT_FACE | DETECT_LEFT_EYE | DETECT_RIGHT_EYE | DETECT_NOSE | DETECT_MOUTH /*| AAM_FIT_FACE*/ | AAM_FIT_MOUTH /*| SAVE_OUTPUT | SAVE_FEATURES*/, 
//		"01.face.2011.12.11.aam_basic", 
//		"04.mouth.color.2012.01.10.aam_basic",
//		"../../_data/" );
//	CvCapture* capture = InitCapture( argc, argv );
//	IplImage* frame = NULL;
//
//	if( capture )
//	{
//		cout << "In capture ..." << endl;
//		
//		while( capture != NULL ) 
//		{
//		    if( ( frame = CaptureFrame( capture ) ) == NULL )
//	            break;
//
//			if( (char)cvWaitKey(10) == 27 )
//				break;
//
//			iface->TrackFeatures(frame);
//
//			FaceTrackerUtil::ShowImage(iface->OutputImage(), "Face tracking from video stream or webcam - iPhone", 1);
//		}
//
//		if( capture )
//			cvReleaseCapture( &capture );
//	}
//	else
//	{
//		FILE* f = NULL;
//
//		cout << "In image read" << endl;
//
//		if( argc == 3 && strcmp(argv[1], "text") == 0 && ( f = fopen( argv[2], "rt" ) ) != NULL )
//		{
//			char buf[1000+1];
//
//			while( fgets( buf, 1000, f ) )
//			{
//				int len = (int)strlen(buf);
//
//				while( len > 0 && isspace(buf[len-1]) )
//					len--;
//
//				buf[len] = '\0';
//
//				cout << "file " << buf << endl;
//
//				if( ( frame = cvLoadImage( buf ) ) != NULL )
//				{
//					iface->TrackFeatures(frame);
//					FaceTrackerUtil::ShowImage(iface->OutputImage(), "Face tracking from single image files - iPhone", 0);
//
//					if( (char)cvWaitKey(10) == 27 )
//						break;
//
//					cvReleaseImage(&frame);
//				}
//				else
//				{
//					cerr << "Aw snap, couldn't read image " << buf << endl;
//				}
//			}
//
//			fclose(f);
//		}
//	}
//
//	cvDestroyAllWindows();
//    delete iface;
//
//	return 0;
//}
//
//void Help( void )
//{
//	cout << "This program illustrates a simple face tracking application for iPhone." << endl;
//	cout << "Usage: ./FaceTracker [video|camera|text] [video filename|camera index]" << endl;
//	cout << "Press ESC to exit." << endl << endl;
//}
//
//CvCapture *InitCapture( int argc, char** argv )
//{
//	CvCapture* capture = NULL;
//
//	if( argc == 1 || (argc == 3 && strcmp(argv[1], "camera") == 0 && strlen(argv[2]) == 1 && isdigit(argv[2][0])))
//	{
//		capture = cvCaptureFromCAM( argc == 3 ? argv[2][0] - '0' : 0 );
//
//		cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 1280 );
//		cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 720 );
//	}
//	else if( argc == 3 && strcmp(argv[1], "video") == 0 )
//	{
//		capture = cvCaptureFromAVI( argv[2] );
//	}
//
//	return capture;
//}
//
//IplImage *CaptureFrame( CvCapture* capture )
//{
//    IplImage* frame = NULL;
//
//    if( ( frame = cvQueryFrame(capture) ) == NULL ) 
//	    return NULL;
//
//	if( frame->origin != IPL_ORIGIN_TL )
//	{
//		cvFlip( frame, frame, 0 );
//		frame->origin = 0;
//	}
//	else
//	{
//		cvFlip( frame, frame, 1 );
//	}
//
//    return frame;
//}
