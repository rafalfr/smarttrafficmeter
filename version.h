#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "18";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2020";
	static const char UBUNTU_VERSION_STYLE[] =  "20.04";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 2;
	static const long BUILD  = 1300;
	static const long REVISION  = 7266;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1326;
	#define RC_FILEVERSION 2,2,1300,7266
	#define RC_FILEVERSION_STRING "2, 2, 1300, 7266\0"
	static const char FULLVERSION_STRING [] = "2.2.1300.7266";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 0;
	

}
#endif //VERSION_H
