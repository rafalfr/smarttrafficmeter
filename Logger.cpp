/*

logger.cpp

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

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <time.h>

#include "Logger.h"

using namespace std;

/** @brief LogDebug
  *
  * @todo: document this function
  */
void Logger::LogDebug( const string& message )
{
    ofstream f;

    f.open( "stm_log.txt", std::ios::app );

    if ( f.good() )
    {
        time_t     now = time( nullptr );
        struct tm  tstruct;
        char       buf[256];
        tstruct = *localtime( &now );

        memset( buf, 0, 256 * sizeof( char ) );

        strftime( buf, sizeof( buf ), "%Y-%m-%d, %X", &tstruct );

        f << "[debug]\t";
        f << buf << "\t";
        f << message;
        f << endl;

        f.close();
    }
}

/** @brief LogInfo
  *
  * @todo: document this function
  */
void Logger::LogInfo( const string& message )
{
    ofstream f;

    f.open( "stm_log.txt", std::ios::app );

    if ( f.good() )
    {
        time_t     now = time( nullptr );
        struct tm  tstruct;
        char       buf[256];
        tstruct = *localtime( &now );

        memset( buf, 0, 256 * sizeof( char ) );

        strftime( buf, sizeof( buf ), "%Y-%m-%d, %X", &tstruct );

        f << "[info]\t";
        f << buf << "\t";
        f << message;
        f << endl;

        f.close();
    }
}


/** @brief LogError
  *
  * @todo: document this function
  */
void Logger::LogError( const string& message )
{
    ofstream f;

    f.open( "stm_log.txt", std::ios::app );

    if ( f.good() )
    {
        time_t     now = time( nullptr );
        struct tm  tstruct;
        char       buf[256];
        tstruct = *localtime( &now );

        memset( buf, 0, 256 * sizeof( char ) );

        strftime( buf, sizeof( buf ), "%Y-%m-%d, %X", &tstruct );

        f << "[error]\t";
        f << buf << "\t";
        f << message;
        f << endl;

        f.close();
    }
}
