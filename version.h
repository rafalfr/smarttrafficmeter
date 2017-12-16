#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "16";
	static const char MONTH[] = "12";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.12";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 1;
	static const long BUILD  = 1208;
	static const long REVISION  = 6780;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1232;
	#define RC_FILEVERSION 2,1,1208,6780
	#define RC_FILEVERSION_STRING "2, 1, 1208, 6780\0"
	static const char FULLVERSION_STRING [] = "2.1.1208.6780";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 8;
	

}
#endif //VERSION_H
