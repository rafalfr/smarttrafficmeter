#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "12";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.07";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 6;
	static const long BUILD  = 658;
	static const long REVISION  = 3754;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 721;
	#define RC_FILEVERSION 1,6,658,3754
	#define RC_FILEVERSION_STRING "1, 6, 658, 3754\0"
	static const char FULLVERSION_STRING [] = "1.6.658.3754";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 58;
	

}
#endif //VERSION_H
