#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "17";
	static const char MONTH[] = "12";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.12";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 1;
	static const long BUILD  = 1209;
	static const long REVISION  = 6784;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1232;
	#define RC_FILEVERSION 2,1,1209,6784
	#define RC_FILEVERSION_STRING "2, 1, 1209, 6784\0"
	static const char FULLVERSION_STRING [] = "2.1.1209.6784";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 9;
	

}
#endif //VERSION_H
