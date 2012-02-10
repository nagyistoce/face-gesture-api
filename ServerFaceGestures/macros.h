#ifndef _MACROS_H_
#define _MACROS_H_

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32) || defined (_WINDOWS)
	#include <opencv2/opencv.hpp>
	#define WINDOWS
	#define FACE_API __declspec(dllexport)

	#define OPENCV_CASCADES		"cascades/"
	#define AAM_MODELS			"AAM.Models/"
#else
	#include <Opencv/opencv2/opencv.hpp>
	#define IOS
	#define FACE_API	

	#define OPENCV_CASCADES		""	
	#define AAM_MODELS			""	
#endif

#define WHITE			255
#define BLACK			0

#define DETECT_FACE			1
#define DETECT_LEFT_EYE		2
#define DETECT_RIGHT_EYE	4
#define DETECT_NOSE			8
#define DETECT_MOUTH		16
#define AAM_FIT_FACE		32
#define AAM_FIT_MOUTH		64
#define SAVE_OUTPUT			128
#define SAVE_FEATURES		256

#define AAM_DEBUG_MODE 0

#endif //_MACROS_H_