#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#define SPZ_STL(a) _##a
#else
	#define SPZ_STL(a) a
#endif