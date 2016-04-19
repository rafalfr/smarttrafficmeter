#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "19";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.04";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 4;
	static const long BUILD  = 417;
	static const long REVISION  = 2375;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 398;
	#define RC_FILEVERSION 1,4,417,2375
	#define RC_FILEVERSION_STRING "1, 4, 417, 2375\0"
	static const char FULLVERSION_STRING [] = "1.4.417.2375";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 17;
	

}
#endif //VERSION_H
