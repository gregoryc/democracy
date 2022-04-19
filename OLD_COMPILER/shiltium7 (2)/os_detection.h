#ifndef _OS_DETECTION
	#define _OS_DETECTION
	
	#ifdef __unix__
		#define UNIX  1
	#else
		#define UNIX  0
	#endif
	
	#if defined _WIN32 || defined _WIN64 || defined WIN32 || defined WIN64
		#define WINDOWS  1
	#else
		#define WINDOWS  0
	#endif
#endif
