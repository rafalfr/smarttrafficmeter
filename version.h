#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "29";
	static const char MONTH[] = "03";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.03";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 1;
	static const long BUILD  = 1285;
	static const long REVISION  = 7196;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1300;
	#define RC_FILEVERSION 2,1,1285,7196
	#define RC_FILEVERSION_STRING "2, 1, 1285, 7196\0"
	static const char FULLVERSION_STRING [] = "2.1.1285.7196";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 85;
	

}
#endif //VERSION_H
