#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "04";
	static const char MONTH[] = "01";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.01";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 9;
	static const long BUILD  = 986;
	static const long REVISION  = 5523;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1037;
	#define RC_FILEVERSION 1,9,986,5523
	#define RC_FILEVERSION_STRING "1, 9, 986, 5523\0"
	static const char FULLVERSION_STRING [] = "1.9.986.5523";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 86;
	

}
#endif //VERSION_H
