#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "12";
	static const char MONTH[] = "01";
	static const char YEAR[] = "2023";
	static const char UBUNTU_VERSION_STYLE[] =  "23.01";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 2;
	static const long BUILD  = 1302;
	static const long REVISION  = 7280;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1330;
	#define RC_FILEVERSION 2,2,1302,7280
	#define RC_FILEVERSION_STRING "2, 2, 1302, 7280\0"
	static const char FULLVERSION_STRING [] = "2.2.1302.7280";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 2;
	

}
#endif //VERSION_H
