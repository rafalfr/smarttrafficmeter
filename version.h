#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "30";
	static const char MONTH[] = "12";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.12";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 1;
	static const long BUILD  = 1248;
	static const long REVISION  = 6999;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1264;
	#define RC_FILEVERSION 2,1,1248,6999
	#define RC_FILEVERSION_STRING "2, 1, 1248, 6999\0"
	static const char FULLVERSION_STRING [] = "2.1.1248.6999";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 48;
	

}
#endif //VERSION_H
