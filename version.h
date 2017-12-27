#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "27";
	static const char MONTH[] = "12";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.12";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 1;
	static const long BUILD  = 1229;
	static const long REVISION  = 6881;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1249;
	#define RC_FILEVERSION 2,1,1229,6881
	#define RC_FILEVERSION_STRING "2, 1, 1229, 6881\0"
	static const char FULLVERSION_STRING [] = "2.1.1229.6881";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 29;
	

}
#endif //VERSION_H
