#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "26";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.04";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 0;
	static const long BUILD  = 1112;
	static const long REVISION  = 6219;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1157;
	#define RC_FILEVERSION 2,0,1112,6219
	#define RC_FILEVERSION_STRING "2, 0, 1112, 6219\0"
	static const char FULLVERSION_STRING [] = "2.0.1112.6219";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 12;
	

}
#endif //VERSION_H
