/*

Debug.cpp

Copyright (C) 2018 Rafał Frączek

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

#include "Debug.h"
#include "Utils.h"
#include "Logger.h"


#ifdef __linux
#include <execinfo.h>
#include <unistd.h>
#include "LinuxUtils.h"
#endif // __linux

/** @brief get_backtrace
  *
  * The method returns the backtrace as a string
  *
  * @param void
  * @return string
  *
  */
string Debug::get_backtrace( void )
{

    string out;

#ifdef _WIN32
    return string( "" );
#endif // _WIN32

#ifdef __linux
    void* array[64];
    size_t size, i;

    size = backtrace( array, 64 );

#ifndef __pi__

    //we skip the first two entries
    //because they refer to the Debug::get_backtrace
    for ( i = 2; i < size; i++ )
    {
        out += LinuxUtils::resolve( ( unsigned long )array[i] );
        out += "\n";
    }

#endif // __pi__

#ifdef __pi__

    char** strs = backtrace_symbols( array, size );

    for ( i = 0; i < size; ++i )
    {
        out += strs[i];
        out += "\n";
    }

    free( strs );

#endif // __pi__

#endif // __linux

    return Utils::trim( out );
}
