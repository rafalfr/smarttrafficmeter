#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "18";
	static const char MONTH[] = "12";
	static const char YEAR[] = "2020";
	static const char UBUNTU_VERSION_STYLE[] =  "20.12";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 2;
	static const long BUILD  = 1301;
	static const long REVISION  = 7271;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1328;
	#define RC_FILEVERSION 2,2,1301,7271
	#define RC_FILEVERSION_STRING "2, 2, 1301, 7271\0"
	static const char FULLVERSION_STRING [] = "2.2.1301.7271";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 1;
	

}
#endif //VERSION_H
