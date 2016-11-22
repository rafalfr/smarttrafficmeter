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

/**
 * @file Debug.h
 * @brief Class definition for the Debug class
 *
 * This file contains the definition of the Debug class.
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

using namespace std;

class Debug
{

public:

    static string get_backtrace( void );

};

#endif // DEBUG_H
