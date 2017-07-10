#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "10";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.07";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 0;
	static const long BUILD  = 1117;
	static const long REVISION  = 6238;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1161;
	#define RC_FILEVERSION 2,0,1117,6238
	#define RC_FILEVERSION_STRING "2, 0, 1117, 6238\0"
	static const char FULLVERSION_STRING [] = "2.0.1117.6238";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 17;
	

}
#endif //VERSION_H
