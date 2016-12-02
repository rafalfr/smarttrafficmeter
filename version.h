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
	static const long BUILD  = 883;
	static const long REVISION  = 4982;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 940;
	#define RC_FILEVERSION 1,8,883,4982
	#define RC_FILEVERSION_STRING "1, 8, 883, 4982\0"
	static const char FULLVERSION_STRING [] = "1.8.883.4982";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 83;
	

}
#endif //VERSION_H
