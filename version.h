#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "08";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.10";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 6;
	static const long BUILD  = 698;
	static const long REVISION  = 3949;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 751;
	#define RC_FILEVERSION 1,6,698,3949
	#define RC_FILEVERSION_STRING "1, 6, 698, 3949\0"
	static const char FULLVERSION_STRING [] = "1.6.698.3949";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 98;
	

}
#endif //VERSION_H
