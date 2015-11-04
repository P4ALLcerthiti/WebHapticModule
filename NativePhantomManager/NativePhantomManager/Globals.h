#ifndef _GLOBALS_H_
#define _GLOBALS_H_

//#define MAKE_LIBRARY

#ifdef MAKE_LIBRARY
	#ifdef GRAPH2OBJ_EXPORTS
		#define GRAPH2OBJ_API __declspec(dllexport)
		#define EXPIMP_TEMPLATE 
	#else
		#define GRAPH2OBJ_API __declspec(dllimport)
		#define EXPIMP_TEMPLATE extern 
	#endif

#else
	#define GRAPH2OBJ_API
#endif

#endif