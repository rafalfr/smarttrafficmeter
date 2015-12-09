#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "08";
	static const char MONTH[] = "12";
	static const char YEAR[] = "2015";
	static const char UBUNTU_VERSION_STYLE[] =  "15.12";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 0;
	static const long BUILD  = 90;
	static const long REVISION  = 500;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 93;
	#define RC_FILEVERSION 1,0,90,500
	#define RC_FILEVERSION_STRING "1, 0, 90, 500\0"
	static const char FULLVERSION_STRING [] = "1.0.90.500";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 90;
	

}
#endif //VERSION_H
