#pragma once

#ifdef _MSC_VER
	#ifdef GOT2D_EXPORTS
		#define G2DAPI __declspec(dllexport)
	#else
		#define G2DAPI __declspec(dllimport)
	#endif
#else
	#ifdef GOT2D_EXPORTS
		#define G2DAPI __attribute__ ((visibility ("default"))) 
	#else
		#define G2DAPI 
	#endif
#endif