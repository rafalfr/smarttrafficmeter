#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "23";
	static const char MONTH[] = "01";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.01";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 1;
	static const long BUILD  = 1251;
	static const long REVISION  = 7010;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1269;
	#define RC_FILEVERSION 2,1,1251,7010
	#define RC_FILEVERSION_STRING "2, 1, 1251, 7010\0"
	static const char FULLVERSION_STRING [] = "2.1.1251.7010";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 51;
	

}
#endif //VERSION_H
