#include <string>
#include <opencv2/opencv.hpp>

#include "Distributor.h"

using namespace std;

//---------------------------------------------------------------------------
// tcpip_server class
//---------------------------------------------------------------------------
// ctor
Distributor::Distributor()
{
}

Distributor::Distributor(streamServer* server, int &res) : m_server(server)
{
	iface = new IFaceTracker( 
		0 | DETECT_FACE | DETECT_LEFT_EYE | DETECT_RIGHT_EYE | DETECT_NOSE | DETECT_MOUTH | AAM_FIT_FACE | AAM_FIT_MOUTH | ESTIMATE_HEAD_POSE /*| SAVE_OUTPUT | SAVE_FEATURES*/, 
		"01.face.2011.12.11.aam_basic", 
		"04.mouth.color.2012.01.10.aam_basic",
		"../../_data/" );

	//cvNamedWindow("tcp_images", 1);
	res = 0;
}	

Distributor::~Distributor()
{
	//cvDestroyWindow("tcp_images");
}

string Distributor::executor()
{	
#ifdef SAVE_TO_FILE
	// ez csak egy teszt kepet elmentett, mar nem kell.
	FILE* pFile = fopen("test.JPG","wb");
	size_t result;
    
	if (pFile==NULL)
    {
        printf("pFile==NULL\n");
        return;
    }
	
	result = fwrite(m_server->m_image_data.buffer,1,m_server->m_image_data.len,pFile);
    
	if (result != m_server->m_image_data.len)
    {
		printf("fwrite failer\n");
	}
	else
		printf("fwrite test.JPG READY\n");
	
	fclose (pFile);
#endif

	// JPG konvertalja IplImage Cv formarumba!
	CvMat		buf = cvMat(m_server->m_image_data_information.width, m_server->m_image_data_information.height, CV_8UC3, m_server->m_image_data.buffer);
	IplImage*	imageIn = cvDecodeImage(&buf); 
	string		res;
	
	if( (imageIn->depth == m_server->m_image_data_information.depth) && 
		(imageIn->nChannels == m_server->m_image_data_information.nChannels) &&
		(imageIn->width == m_server->m_image_data_information.width) && 
		(imageIn->height == m_server->m_image_data_information.height))
	{		
		iface->TrackFeatures(imageIn, m_server->m_image_data_information.pTracking, m_server->m_image_data_information.pExpression);

		// IplImage Cv kepet megjeleniti!
		cvShowImage("tcp_images",iface->OutputImage());
		string messg = "barmi uzenet.....!";
		_itoa(m_server->m_image_data_information.frame_count,cTemp,10);
		res = "frame(" + string(cTemp) + "): " +  iface->Message();
	}
	else
	{		
		if (m_server->m_image_data.buffer)
			free (m_server->m_image_data.buffer);

		res = "ERR2";		
	}

	if (imageIn)
		cvReleaseImage(&imageIn);

	return res;
}
