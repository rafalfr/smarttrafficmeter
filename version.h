#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "13";
	static const char MONTH[] = "05";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.05";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 0;
	static const long BUILD  = 1116;
	static const long REVISION  = 6235;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1161;
	#define RC_FILEVERSION 2,0,1116,6235
	#define RC_FILEVERSION_STRING "2, 0, 1116, 6235\0"
	static const char FULLVERSION_STRING [] = "2.0.1116.6235";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 16;
	

}
#endif //VERSION_H
