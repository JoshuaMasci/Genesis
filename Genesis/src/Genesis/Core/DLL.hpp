#pragma once

#ifdef WIN32
	#ifdef GENESIS_BUILD_DLL
		#define  GENESIS_DLL __declspec(dllexport)
	#else
		#define  GENESIS_DLL __declspec(dllimport)
	#endif
#else
	#define  GENESIS_DLL
#endif