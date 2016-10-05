/*

Debug.h

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

#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>


//http://www.opensource.apple.com/source/gdb/gdb-1515/src/binutils/addr2line.c
//https://github.com/dinhviethoa/linux-sample/blob/master/etpan-symbols.c
//https://gist.github.com/jvranish/4441299
//http://stackoverflow.com/questions/8278691/how-to-fix-backtrace-line-number-error-in-c

//http://opensource.apple.com/source/X11libs/X11libs-40.2/cairo/cairo-1.8.6/util/backtrace-symbols.c?txt

//https://panthema.net/2008/0901-stacktrace-demangled/
//http://webcache.googleusercontent.com/search?q=cache:rcg0-9FvfdIJ:https://en.wikibooks.org/wiki/Linux_Applications_Debugging_Techniques/The_call_stack+&cd=3&hl=pl&ct=clnk&gl=pl
//https://github.com/adarqui/darqbot/blob/master/test/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes
//http://spin.atomicobject.com/2013/01/13/exceptions-stack-traces-c/


using namespace std;

class Debug
{

public:

	static string get_backtrace( void );

};

#endif // DEBUG_H
