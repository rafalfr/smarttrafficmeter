/*

version.h

Copyright (C) 2016 Rafał Frączek

This file is part of Smart Traffic Meter.

Smart Traffic Meter is free software:
you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

Smart Traffic Meter is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Smart Traffic Meter.
If not, see http://www.gnu.org/licenses/.

*/

#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "23";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.07";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 6;
	static const long BUILD  = 680;
	static const long REVISION  = 3852;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 735;
	#define RC_FILEVERSION 1,6,680,3852
	#define RC_FILEVERSION_STRING "1, 6, 680, 3852\0"
	static const char FULLVERSION_STRING [] = "1.6.680.3852";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 80;
	

}
#endif //VERSION_H
