#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "13";
	static const char MONTH[] = "11";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.11";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 0;
	static const long BUILD  = 1190;
	static const long REVISION  = 6662;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1217;
	#define RC_FILEVERSION 2,0,1190,6662
	#define RC_FILEVERSION_STRING "2, 0, 1190, 6662\0"
	static const char FULLVERSION_STRING [] = "2.0.1190.6662";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 90;
	

}
#endif //VERSION_H
