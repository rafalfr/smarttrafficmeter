/*

Logger.cpp

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

#include <ctime>
#include <istream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <time.h>

#include "config.h"
#include "Logger.h"
#include "Settings.h"

using namespace std;

/** @brief LogDebug
  *
  * The method logs debug information
  *
  * @param debug message
  * @return void
  *
  */
void Logger::LogDebug( const string& message )
{
    bool remove_log_file = false;

    const string& log_file_path = Settings::settings["log directory"] + PATH_SEPARATOR + "stm_log.txt";

    ifstream in;

    in.open( log_file_path, std::ifstream::binary );

    if ( in.good() )
    {
        in.seekg( 0, in.end );
        streampos length = in.tellg();
        in.seekg( 0, in.beg );
        in.close();

        if ( static_cast<uint64_t>( length ) >= max_log_length )
        {
            remove_log_file = true;
        }
    }

    if ( remove_log_file == true )
    {
        remove( log_file_path.c_str() );
    }

    ofstream f;

    f.open( log_file_path, std::ios::app );

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
  * The method logs standard information
  *
  * @param standard message
  * @return void
  *
  */
void Logger::LogInfo( const string& message )
{
    bool remove_log_file = false;

    const string& log_file_path = Settings::settings["log directory"] + PATH_SEPARATOR + "stm_log.txt";

    ifstream in;

    in.open( log_file_path, std::ifstream::binary );

    if ( in.good() )
    {
        in.seekg( 0, in.end );
        streampos length = in.tellg();
        in.seekg( 0, in.beg );
        in.close();

        if ( static_cast<uint64_t>( length ) >= max_log_length )
        {
            remove_log_file = true;
        }
    }

    if ( remove_log_file == true )
    {
        remove( log_file_path.c_str() );
    }

    ofstream f;

    f.open( log_file_path, std::ios::app );

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
  * The method error standard information
  *
  * @param error message
  * @return void
  *
  */
void Logger::LogError( const string& message )
{
    bool remove_log_file = false;

    const string& log_file_path = Settings::settings["log directory"] + PATH_SEPARATOR + "stm_log.txt";

    ifstream in;

    in.open( log_file_path, std::ifstream::binary );

    if ( in.good() )
    {
        in.seekg( 0, in.end );
        streampos length = in.tellg();
        in.seekg( 0, in.beg );
        in.close();

        if ( static_cast<uint64_t>( length ) >= max_log_length )
        {
            remove_log_file = true;
        }
    }

    if ( remove_log_file == true )
    {
        remove( log_file_path.c_str() );
    }

    ofstream f;

    f.open( log_file_path, std::ios::app );

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
