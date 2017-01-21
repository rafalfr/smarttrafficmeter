#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "21";
	static const char MONTH[] = "01";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.01";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 10;
	static const long BUILD  = 1028;
	static const long REVISION  = 5756;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1081;
	#define RC_FILEVERSION 1,10,1028,5756
	#define RC_FILEVERSION_STRING "1, 10, 1028, 5756\0"
	static const char FULLVERSION_STRING [] = "1.10.1028.5756";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 28;
	

}
#endif //VERSION_H
