#pragma once

#define FACE_API __declspec(dllexport)

#ifdef CORE_DLL
    #define CORE_API __declspec(dllexport)
#else
    #define CORE_API __declspec(dllimport)
#endif

#pragma warning(disable : 4251 4275) /* needs to have dll-interface */
