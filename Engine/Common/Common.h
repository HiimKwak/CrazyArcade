#pragma once
#pragma warning(disable: 4251)
#pragma warning(disable: 4172)

//#define ENGINE_BUILD_DLL 1

#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)

#if ENGINE_BUILD_DLL
#define WANTED_API DLLEXPORT
#else
#define WANTED_API DLLIMPORT
#endif