#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "02";
	static const char MONTH[] = "12";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.12";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 8;
	static const long BUILD  = 879;
	static const long REVISION  = 4959;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 937;
	#define RC_FILEVERSION 1,8,879,4959
	#define RC_FILEVERSION_STRING "1, 8, 879, 4959\0"
	static const char FULLVERSION_STRING [] = "1.8.879.4959";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 79;
	

}
#endif //VERSION_H
