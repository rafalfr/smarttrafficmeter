#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "20";
	static const char MONTH[] = "02";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.02";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 1;
	static const long BUILD  = 1256;
	static const long REVISION  = 7033;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1273;
	#define RC_FILEVERSION 2,1,1256,7033
	#define RC_FILEVERSION_STRING "2, 1, 1256, 7033\0"
	static const char FULLVERSION_STRING [] = "2.1.1256.7033";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 56;
	

}
#endif //VERSION_H
