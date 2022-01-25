#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "25";
	static const char MONTH[] = "01";
	static const char YEAR[] = "2022";
	static const char UBUNTU_VERSION_STYLE[] =  "22.01";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 2;
	static const long BUILD  = 1301;
	static const long REVISION  = 7271;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1330;
	#define RC_FILEVERSION 2,2,1301,7271
	#define RC_FILEVERSION_STRING "2, 2, 1301, 7271\0"
	static const char FULLVERSION_STRING [] = "2.2.1301.7271";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 1;
	

}
#endif //VERSION_H
