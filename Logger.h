/*

Logger.h

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
 * @file Logger.h
 * @brief Class definition for the Logger
 *
 * This file contains the definition of the Logger class.
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <string>

using namespace std;

class Logger
{

public:

	static void LogDebug( const string& );

	static void LogInfo( const string& );

	static void LogError( const string& );
};

#endif // LOGGER_H
