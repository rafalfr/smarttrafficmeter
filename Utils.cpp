/*

Utils.cpp

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

#include "config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <boost/filesystem.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>

#ifdef __linux
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/file.h>
#include <errno.h>
#include "LinuxUtils.h"
#include "Settings.h"
#endif // __linux


#ifdef _WIN32
#include "WindowsUtils.h"
#endif // _WIN32


#ifdef use_sqlite
#include "sqlite3.h"
#endif // use_sqlite

#include "Utils.h"
#include "Logger.h"
#include "Globals.h"

using namespace std;

/**<
The map object that stores results returned by sqlite engine
 */
map<string, string> Utils::table_columns;

/** @brief get_all_interfaces
 *
 * The method returns all available network interfaces
 *
 * @param void
 * @return all available network interfaces
 *
 */
map<string, InterfaceInfo> Utils::get_all_interfaces( void )
{
#ifdef _WIN32
    return WindowsUtils::get_all_interfaces();
#endif // _WIN32

#ifdef __linux
    return LinuxUtils::get_all_interfaces();
#endif // __linux
}

/** @brief get_mac
 *
 * The method returns the MAC address of a network interface
 *
 * @param network interface name
 * @return MAC address
 *
 */
string Utils::get_mac( char* name )
{
#ifdef _WIN32

#endif // _WIN32

#ifdef __linux
    return LinuxUtils::get_mac( name );
#endif // __linux
}

/** @brief get_user_host
 *
 * The method returns the user name and host name
 *
 * @param[out] user name
 * @param[out] host name
 * @return void
 *
 */
void Utils::get_user_host( string& user, string& host )
{
#ifdef _WIN32

#endif // _WIN32

#ifdef __linux
    return LinuxUtils::get_user_host( user, host );
#endif // __linux
}

/** @brief get_os_info
 *
 * The method returns the operating system name
 *
 * @param[out] operating system name
 * @return void
 *
 */
void Utils::get_os_info( string& os_info )
{
#ifdef _WIN32

#endif // _WIN32

#ifdef __linux
    return LinuxUtils::get_os_info( os_info );
#endif // __linux
}

/** @brief get_time
 *
 * The method returns the current time
 *
 * @param[out] current year
 * @param[out] current month
 * @param[out] current day
 * @param[out] current hour
 * @return void
 *
 */
void Utils::get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h )
{
    time_t t = time( nullptr );
    struct tm* tm = localtime( &t );
    ( *y ) = tm->tm_year + 1900;
    ( *m ) = tm->tm_mon + 1;
    ( *d ) = tm->tm_mday;
    ( *h ) = tm->tm_hour;
}

/** @brief get_time
 *
 * The method returns the current time
 *
 * @param[out] current year
 * @param[out] current month
 * @param[out] current day
 * @param[out] current hour
 * @param[out] current minute
 * @return void
 *
 */
void Utils::get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h, uint32_t* minute )
{
    time_t t = time( nullptr );
    struct tm* tm = localtime( &t );
    ( *y ) = tm->tm_year + 1900;
    ( *m ) = tm->tm_mon + 1;
    ( *d ) = tm->tm_mday;
    ( *h ) = tm->tm_hour;
    ( *minute ) = tm->tm_min;
}

/** @brief date_to_seconds
 *
 * The method converts the current time to number of seconds since the epoch
 *
 * @param year
 * @param month
 * @param day
 * @param hour
 * @param minute
 * @param second
 * @return number of seconds since the epoch (1970-01-01 00:00:00 +0000 (UTC))
 *
 */
time_t Utils::date_to_seconds( uint32_t y, uint32_t m, uint32_t d, uint32_t h, uint32_t minute, uint32_t sec )
{
    time_t t = time( nullptr );
    struct tm* tm = localtime( &t );
    tm->tm_year = y - 1900;
    tm->tm_mon = m - 1;
    tm->tm_mday = d;
    tm->tm_hour = h;
    tm->tm_min = minute;
    tm->tm_sec = sec;
    tm->tm_wday = 0;
    tm->tm_yday = 0;
    tm->tm_isdst = 0;
    return mktime( tm );
}


/** @brief seconds_to_date
 *
 * The method converts the time in seconds since the epoch to the date.
 *
 * @param number of seconds since the epoch (1970-01-01 00:00:00 +0000 (UTC))
 * @param[out] year
 * @param[out] month
 * @param[out] day
 * @param[out] hour
 * @return void
 *
 */
void Utils::seconds_to_date( time_t t, uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h )
{
    struct tm* tm = localtime( &t );
    ( *y ) = tm->tm_year + 1900;
    ( *m ) = tm->tm_mon + 1;
    ( *d ) = tm->tm_mday;
    ( *h ) = tm->tm_hour;
}

/** @brief contains
  *
  * The function returns true if key is found in the str, otherwise false is returned.
  *
  * @param string in which search will be performed
  * @param key string which will be searched
  * @return True if str contains key
  *
  */
bool Utils::contains( const string& str, const string& key )
{
    if ( str.find( key ) != string::npos )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/** @brief starts_with
  *
  * The function returns true if str starts with key
  *
  * @param string in which search will be performed
  * @param key string which will be searched
  * @return True if str starts with key
  *
  */
bool Utils::starts_with( const string& str, const string& key )
{
    return str.size() >= key.size() && str.compare(0, key.size(), key) == 0;
}

/** @brief split
  *
  * The function splits the input string using the given delimiter
  *
  * @param string which will be split
  * @param delimiter
  * @return vector of obtained parts of the input string
  *
  */
vector<string> Utils::split( const string& str, const string& delim )
{
    size_t start;
    size_t end;

    vector<string> items;

    if ( str.size() == 0  || delim.size() == 0 )
    {
        items.push_back( str );
        return items;
    }

    start = 0;
    end = str.find( delim );

    while ( end != string::npos )
    {
        items.push_back( str.substr( start, end - start ) );
        start = end + delim.length();
        end = str.find( delim, start );
    }

    items.push_back( str.substr( start ) );

    return items;
}

/** @brief replace
  *
  * The function replaces the pattern with a new content in the given string
  *
  * @param pattern
  * @param content that will replace the pattern
  * @param input string
  * @return new string with pattern replaced with the content
  *
  */
string Utils::replace( const string& pattern, const string& with, const string& in ) noexcept
{

    string out;

    size_t start = 0;

    if ( in.find( pattern, start ) == string::npos )
    {
        out += in;
    }
    else
    {
    	size_t end;
        while ( ( end = in.find( pattern, start ) ) != string::npos )
        {
            out += in.substr( start, end - start );

            out += with;

            start = end + pattern.size();
        }

        out += in.substr( start, end );
    }

    return out;
}

/** @brief float_to_string
  *
  * The function converts float value to string
  *
  * @param value
  * @param string precision
  * @return string representation of the float value
  *
  */
string Utils::float_to_string( float value, int32_t precision )
{
    std::ostringstream stm ;
    stm.precision( precision );
    stm.setf( std::ios_base::fixed );
    stm << value ;
    return stm.str() ;
}

/** @brief to_string
  *
  * The function converts uint64_t value to string
  * If the string representation is shorter than min_string_length
  * the string is prepended with the appropriate number of zeros.
  *
  * @param uint64_t value
  * @param minimum string length
  * @return string representation of the uint64_t value
  *
  */
string Utils::to_string( uint64_t value, uint32_t min_string_length )
{

    string out;

    uint64_t divisor = 10000000000000000000ULL;

    if ( value == 0ULL )
    {
        out += "0";
    }
    else
    {
        while ( divisor > 0ULL )
        {
            uint64_t c = value / divisor;
            value -= c * divisor;
            divisor /= 10ULL;

            if ( out.empty() == false || c > 0ULL )
            {
                if ( c == 0ULL )
                {
                    out += "0";
                }
                else if ( c == 1ULL )
                {
                    out += "1";
                }
                else if ( c == 2ULL )
                {
                    out += "2";
                }
                else if ( c == 3ULL )
                {
                    out += "3";
                }
                else if ( c == 4ULL )
                {
                    out += "4";
                }
                else if ( c == 5ULL )
                {
                    out += "5";
                }
                else if ( c == 6ULL )
                {
                    out += "6";
                }
                else if ( c == 7ULL )
                {
                    out += "7";
                }
                else if ( c == 8ULL )
                {
                    out += "8";
                }
                else if ( c == 9ULL )
                {
                    out += "9";
                }
            }
        }
    }

    for ( int32_t i = 0; i < ( ( int32_t ) min_string_length - ( int32_t ) out.size() ); i++ )
    {
        out.insert( 0, "0" );
    }

    return out;
}


/** @brief to_string
  *
  * The function converts uint32_t value to string
  * If the string representation is shorter than min_string_length
  * the string is prepended with the appropriate number of zeros.
  *
  * @param uint64_t value
  * @param minimum string length
  * @return string representation of the uint32_t value
  *
  */
string Utils::to_string( uint32_t value, uint32_t min_string_length )
{

    string out;

    uint32_t divisor = 1000000000U;

    if ( value == 0U )
    {
        out += "0";
    }
    else
    {
        while ( divisor > 0U )
        {
            uint32_t c = value / divisor;
            value -= c * divisor;
            divisor /= 10U;

            if ( out.empty() == false || c > 0U )
            {
                if ( c == 0U )
                {
                    out += "0";
                }
                else if ( c == 1U )
                {
                    out += "1";
                }
                else if ( c == 2U )
                {
                    out += "2";
                }
                else if ( c == 3U )
                {
                    out += "3";
                }
                else if ( c == 4U )
                {
                    out += "4";
                }
                else if ( c == 5U )
                {
                    out += "5";
                }
                else if ( c == 6U )
                {
                    out += "6";
                }
                else if ( c == 7U )
                {
                    out += "7";
                }
                else if ( c == 8U )
                {
                    out += "8";
                }
                else if ( c == 9U )
                {
                    out += "9";
                }
            }
        }
    }

    for ( int32_t i = 0; i < ( ( int32_t ) min_string_length - ( int32_t ) out.size() ); i++ )
    {
        out.insert( 0, "0" );
    }

    return out;
}

/** @brief hexcolor_to_strings
  *
  * The function converts the color representation as hex string
  * to a vector of three strings each representing R, G, B channels respectively
  *
  * @param string
  * @return vector of strings
  *
  */
vector<string> Utils::hexcolor_to_strings( string& hex_color )
{
    vector<string> out;

    if ( hex_color.size() != 6 )
    {
        out.push_back( "0" );
        out.push_back( "0" );
        out.push_back( "0" );
        return out;
    }


    uint32_t color;

    string color_item_str;

    // r
    color_item_str = hex_color.substr( 0, 2 );

    color = Utils::hstoi( color_item_str );

    out.push_back( Utils::to_string( color ) );

    // g
    color_item_str = hex_color.substr( 2, 2 );

    color = Utils::hstoi( color_item_str );

    out.push_back( Utils::to_string( color ) );

    // b
    color_item_str = hex_color.substr( 4, 2 );

    color = Utils::stoi( color_item_str );

    out.push_back( Utils::to_string( color ) );

    return out;
}

/** @brief check_one_instance
 *
 * The method checks whether another instance of the program is currently running
 *
 * @param void
 * @return true if this instance is the only one currently running, otherwise false is returned
 *
 */
bool Utils::check_one_instance( void )
{
#ifdef _WIN32
    return WindowsUtils::check_one_instance();
#endif // _WIN32

#ifdef __linux
    return LinuxUtils::check_one_instance();
#endif // __linux
}


/** @brief remove_instance_object
 *
 * The method removes from the os a shared object which represents this program instance
 *
 * @param void
 * @return void
 *
 */
void Utils::remove_instance_object( void )
{
#ifdef __linux
    LinuxUtils::remove_instance_object();
#endif // __linux
}


/** @brief trim
 *
 * The method trims the given string
 *
 * @param string
 * @return trimmed string
 *
 */
string Utils::trim( const std::string& s )
{
    auto wsfront = std::find_if_not( s.begin(), s.end(), []( int c )
    {
        return std::isspace( c );
    } );

    auto wsback = std::find_if_not( s.rbegin(), s.rend(), []( int c )
    {
        return std::isspace( c );
    } ).base();

    return ( wsback <= wsfront ? std::string() : std::string( wsfront, wsback ) );
}


/** @brief date_str
 *
 * The method converts the date to a string representation which is used in the database
 *
 * @param data type (e.g. yearly, monthly, daily, hourly)
 * @param year
 * @param month
 * @param day
 * @param hour
 * @return date string representation
 *
 */
string Utils::date_str( const string& type, uint32_t y, uint32_t m, uint32_t d, uint32_t h )
{
    if ( type.compare( "yearly" ) == 0 )
    {
        return Utils::to_string( y );
    }
    else if ( type.compare( "monthly" ) == 0 )
    {
        return Utils::to_string( y ) + "-" + Utils::to_string( m, 2 );
    }
    else if ( type.compare( "daily" ) == 0 )
    {
        return Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );
    }
    else if ( type.compare( "hourly" ) == 0 )
    {
        return Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";
    }

    return "";
}

/** @brief str2date
 *
 * The method converts the given string to date
 * The method performs the inverse conversion to the date_str method
 *
 * @param date string representation
 * @param data type (e.g. yearly, monthly, daily, hourly)
 * @param[out] year
 * @param[out] month
 * @param[out] day
 * @param[out] hour
 * @return none
 *
 */
void Utils::str2date( const string& str, const string& type, uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h )
{
    if ( type.compare( "yearly" ) == 0 )
    {
        ( *y ) = Utils::stoui( str );
        ( *m ) = 1U;
        ( *d ) = 1U;
        ( *h ) = 0U;

        if ( ( *y ) == 0U )
        {
            ( *y ) = 0U;
            ( *m ) = 0U;
            ( *d ) = 0U;
            ( *h ) = 0U;
        }
    }
    else if ( type.compare( "monthly" ) == 0 )
    {
        const vector<string>& items = Utils::split( str, "-" );

        if ( items.size() == 2 )
        {
            ( *y ) = Utils::stoui( items[0] );
            ( *m ) = Utils::stoui( items[1] );
            ( *d ) = 1U;
            ( *h ) = 0U;
        }
        else
        {
            ( *y ) = 0U;
            ( *m ) = 0U;
            ( *d ) = 0U;
            ( *h ) = 0U;
        }
    }
    else if ( type.compare( "daily" ) == 0 )
    {
        const vector<string>& items = Utils::split( str, "-" );

        if ( items.size() == 3 )
        {
            ( *y ) = Utils::stoui( items[0] );
            ( *m ) = Utils::stoui( items[1] );
            ( *d ) = Utils::stoui( items[2] );
            ( *h ) = 0U;
        }
        else
        {
            ( *y ) = 0U;
            ( *m ) = 0U;
            ( *d ) = 0U;
            ( *h ) = 0U;
        }
    }
    else if ( type.compare( "hourly" ) == 0 )
    {
        const vector<string>& items1 = Utils::split( str, "_" );
        const vector<string>& items = Utils::split( items1[0], "-" );

        if ( items1.size() == 2 && items.size() == 3 )
        {
            ( *y ) = Utils::stoui( items[0] );
            ( *m ) = Utils::stoui( items[1] );
            ( *d ) = Utils::stoui( items[2] );
            const vector<string>& items3 = Utils::split( items1[1], "-" );
            ( *h ) = Utils::stoui( items3[0].substr( 0, 2 ) );
        }
        else
        {
            ( *y ) = 0U;
            ( *m ) = 0U;
            ( *d ) = 0U;
            ( *h ) = 0U;
        }
    }
}

/** @brief days_in_month
  *
  * @todo: document this function
  */
uint32_t Utils::days_in_month( uint32_t month, uint32_t year )
{
    if ( month == 0U || month == 1UL || month == 3UL || month == 5UL || month == 7UL || month == 8UL || month == 10UL || month == 12UL )
    {
        return 31UL;
    }
    else if ( month == 4UL || month == 6UL || month == 9UL || month == 11UL )
    {
        return 30UL;
    }
    else if ( month == 2UL )
    {
        bool leap_year = ( year % 400 ) ? ( ( year % 100 ) ? ( ( year % 4 ) ? false : true ) : false ) : true;

        if ( leap_year )
        {
            return 29UL;
        }
        else
        {
            return 28UL;
        }
    }
    else
    {
        return 0UL;
    }
}


/** @brief load_stats
  *
  * @todo: document this function
  */
void Utils::load_stats( const string& a_mac )
{
    const string& storage = Settings::settings["storage"];

    if ( Utils::starts_with( storage, "mysql" ) )
    {
#ifdef use_mysql
#endif // use_mysql
    }

    if ( Utils::starts_with( storage, "sqlite" ) )
    {
#ifdef use_sqlite
        Utils::load_data_from_sqlite( a_mac );
#endif // use_sqlite
    }

    if ( Utils::starts_with( storage, "files" ) )
    {
        Utils::load_data_from_files( a_mac );
    }
}


/** @brief load_data_from_sqlite
  *
  * The method loads data from the sqlite database
  * The database file is determined based on
  * the current working directory and network interface MAC address.
  *
  * @param mac
  * @return void
  *
  */
void Utils::load_data_from_sqlite( const string& a_mac )
{
#ifdef use_sqlite

    Globals::data_load_save_mutex.lock();

    sqlite3 *db;
    char *zErrMsg = nullptr;
    uint32_t y;
    uint32_t m;
    uint32_t d;
    uint32_t h;
    uint64_t rx_bytes;
    uint64_t tx_bytes;
    vector<string> macs;

    Utils::get_time( &y, &m, &d, &h );

    macs.clear();

    if ( a_mac.compare( "" ) == 0 )
    {
        const map<string, InterfaceInfo>& interfaces = Utils::get_all_interfaces();

        for ( auto const & kv : interfaces )
        {
            const InterfaceInfo& in = kv.second;
            const string& mac = in.get_mac();
            macs.push_back( mac );
        }
    }
    else
    {
        macs.push_back( a_mac );
    }

    string row;
    string query;

    for ( string const & mac : macs )
    {
        int rc = sqlite3_open_v2( ( Settings::settings["database directory"] + PATH_SEPARATOR + mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE, nullptr );

        if ( rc != SQLITE_OK )
        {
            continue;
        }

        query.clear();
        query += "DELETE FROM yearly WHERE row NOT LIKE '2%%';";
        rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

        if ( rc != SQLITE_OK )
        {
            sqlite3_free( zErrMsg );
            Logger::LogError( "Can not delete empty row in the yearly table" );
        }

        query.clear();
        query += "DELETE FROM yearly WHERE rx_bytes='0' AND tx_bytes='0';";
        rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

        if ( rc != SQLITE_OK )
        {
            sqlite3_free( zErrMsg );
            Logger::LogError( "Can not delete from yearly table rows with zero rx and tx values" );
        }

        query.clear();
        query += "SELECT * from yearly ";
        query += "WHERE row=";
        query += "'";
        query += Utils::to_string( y );
        query += "'";
        query += ";";
        table_columns.clear();
        rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

        if ( rc == SQLITE_OK )
        {
            row = table_columns["row"];

            try
            {
                rx_bytes = Utils::stoull( table_columns["rx_bytes"] );
            }
            catch ( ... )
            {
                rx_bytes = 0ULL;
            }

            try
            {
                tx_bytes = Utils::stoull( table_columns["tx_bytes"] );
            }
            catch ( ... )
            {
                tx_bytes = 0ULL;
            }

            InterfaceStats ystats;
            Globals::all_stats[mac]["yearly"][row] = ystats;
            Globals::all_stats[mac]["yearly"][row].set_initial_stats( tx_bytes, rx_bytes );
        }

///
        query.clear();
        query += "DELETE FROM monthly WHERE row NOT LIKE '2%%';";
        rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

        if ( rc != SQLITE_OK )
        {
            sqlite3_free( zErrMsg );
            Logger::LogError( "Can not delete empty row in the monthly table" );
        }

        query.clear();
        query += "DELETE FROM monthly WHERE rx_bytes='0' AND tx_bytes='0';";
        rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

        if ( rc != SQLITE_OK )
        {
            sqlite3_free( zErrMsg );
            Logger::LogError( "Can not delete from monthly table rows with zero rx and tx values" );
        }

        query.clear();
        query += "SELECT * from monthly ";
        query += "WHERE row=";
        query += "'";
        query += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 );
        query += "'";
        query += ";";
        table_columns.clear();
        rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

        if ( rc == SQLITE_OK )
        {
            row = table_columns["row"];

            try
            {
                rx_bytes = Utils::stoull( table_columns["rx_bytes"] );
            }
            catch ( ... )
            {
                rx_bytes = 0ULL;
            }

            try
            {
                tx_bytes = Utils::stoull( table_columns["tx_bytes"] );
            }
            catch ( ... )
            {
                tx_bytes = 0ULL;
            }

            InterfaceStats mstats;
            Globals::all_stats[mac]["monthly"][row] = mstats;
            Globals::all_stats[mac]["monthly"][row].set_initial_stats( tx_bytes, rx_bytes );
        }

///
        query.clear();
        query += "DELETE FROM daily WHERE row NOT LIKE '2%%';";
        rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

        if ( rc != SQLITE_OK )
        {
            sqlite3_free( zErrMsg );
            Logger::LogError( "Can not delete empty row in the daily table" );
        }

        query.clear();
        query += "DELETE FROM daily WHERE rx_bytes='0' AND tx_bytes='0';";
        rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

        if ( rc != SQLITE_OK )
        {
            sqlite3_free( zErrMsg );
            Logger::LogError( "Can not delete from daily table rows with zero rx and tx values" );
        }

        query.clear();
        query += "SELECT * from daily ";
        query += "WHERE row=";
        query += "'";
        query += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );
        query += "'";
        query += ";";

        table_columns.clear();
        rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

        if ( rc == SQLITE_OK )
        {
            row = table_columns["row"];

            try
            {
                rx_bytes = Utils::stoull( table_columns["rx_bytes"] );
            }
            catch ( ... )
            {
                rx_bytes = 0ULL;
            }

            try
            {
                tx_bytes = Utils::stoull( table_columns["tx_bytes"] );
            }
            catch ( ... )
            {
                tx_bytes = 0ULL;
            }

            InterfaceStats dstats;
            Globals::all_stats[mac]["daily"][row] = dstats;
            Globals::all_stats[mac]["daily"][row].set_initial_stats( tx_bytes, rx_bytes );
        }

///
        query.clear();
        query += "DELETE FROM hourly WHERE row NOT LIKE '2%%';";
        rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

        if ( rc != SQLITE_OK )
        {
            sqlite3_free( zErrMsg );
            Logger::LogError( "Can not delete empty row in the hourly table" );
        }

        query.clear();
        query += "DELETE FROM hourly WHERE rx_bytes='0' AND tx_bytes='0';";
        rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

        if ( rc != SQLITE_OK )
        {
            sqlite3_free( zErrMsg );
            Logger::LogError( "Can not delete from hourly table rows with zero rx and tx values" );
        }

        query.clear();
        query += "SELECT * from hourly ";
        query += "WHERE row=";
        query += "'";
        query += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";
        query += "'";
        query += ";";

        table_columns.clear();
        rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

        if ( rc == SQLITE_OK )
        {
            row = table_columns["row"];

            try
            {
                rx_bytes = Utils::stoull( table_columns["rx_bytes"] );
            }
            catch ( ... )
            {
                rx_bytes = 0ULL;
            }

            try
            {
                tx_bytes = Utils::stoull( table_columns["tx_bytes"] );
            }
            catch ( ... )
            {
                tx_bytes = 0ULL;
            }

            InterfaceStats hstats;
            Globals::all_stats[mac]["hourly"][row] = hstats;
            Globals::all_stats[mac]["hourly"][row].set_initial_stats( tx_bytes, rx_bytes );
        }

        sqlite3_close_v2( db );
    }

    Globals::data_load_save_mutex.unlock();

#endif // use_sqlite
}

/** @brief load_data_from_files
  *
  * The method loads data from the files database
  * The database file is determined based on
  * the current working directory and network interface MAC address.
  *
  * @param mac
  * @return void
  *
  */
void Utils::load_data_from_files( const string& )
{
}


/** @brief save_stats
  *
  * @todo: document this function
  */
void Utils::save_stats( const string& a_mac )
{
    const string& storage = Settings::settings["storage"];

    if ( Utils::contains( storage, "mysql" ) )
    {
#ifdef use_mysql
        //save_stats_to_mysql(a_mac);
#endif // use_mysql
    }

    if ( Utils::contains( storage, "sqlite" ) )
    {
#ifdef use_sqlite
        Utils::save_stats_to_sqlite( a_mac );
#endif // use_sqlite
    }

    if ( Utils::contains( storage, "files" ) )
    {
        Utils::save_stats_to_files( a_mac );
    }
}


/** @brief save_stats_to_sqlite
  *
  * The method saves the statistics to the sqlite database
  * The database file is determined based on
  * the current working directory and network interface MAC address.
  *
  * @param mac
  * @return void
  *
  */
void Utils::save_stats_to_sqlite( const string& a_mac )
{

#ifdef use_sqlite

    Globals::data_load_save_mutex.lock();

    vector<string> macs;

    macs.clear();

    if ( a_mac.compare( "" ) == 0 )
    {
        for ( auto const & mac_table : Globals::all_stats )
        {
            const string& mac = mac_table.first;
            macs.push_back( mac );
        }
    }
    else
    {
        macs.push_back( a_mac );
    }


    string query;

    for ( auto const & mac : macs )
    {
        sqlite3 *db;
        char *zErrMsg = nullptr;
        int rc;

        const map<string, map<string, InterfaceStats> > & table = Globals::all_stats[mac];

        rc = sqlite3_open_v2( ( Settings::settings["database directory"] + PATH_SEPARATOR + mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr );

        if ( rc != SQLITE_OK )
        {
            continue;
        }

        for ( auto const & table_row : table )
        {
            const string& table_name = table_row.first;

            query.clear();
            query += "CREATE TABLE IF NOT EXISTS '" + table_name + "' ('row' VARCHAR(45) NULL,'rx_bytes' UNSIGNED BIG INT NULL,'tx_bytes' UNSIGNED BIG INT NULL,PRIMARY KEY ('row'));";

            rc = sqlite3_exec( db, query.c_str(), Utils::callback, nullptr, &zErrMsg );

            if ( rc != SQLITE_OK )
            {
                sqlite3_free( zErrMsg );
                Logger::LogError( "Can not create " + table_name );
                continue;
            }

            const map<string, InterfaceStats> & row = table_row.second;

            for ( auto const & row_stats : row )
            {
                const string& row = row_stats.first;
                const InterfaceStats& stats = row_stats.second;
                uint64_t rx = stats.received();
                uint64_t tx = stats.transmitted();

                query.clear();
                query += "INSERT OR IGNORE INTO " + table_name + " (row,rx_bytes,tx_bytes) VALUES(";
                query += "'";
                query += row;
                query += "'";
                query += ",";
                query += Utils::to_string( rx );
                query += ",";
                query += Utils::to_string( tx );
                query += ");";
                rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

                if ( rc != SQLITE_OK )
                {
                    sqlite3_free( zErrMsg );
                    Logger::LogError( "Can not insert row: " + row + " into the table " + table_name );
                    continue;
                }

                query.clear();
                query += "UPDATE OR IGNORE " + table_name + " SET ";
                query += "rx_bytes=";
                query += Utils::to_string( rx );
                query += ", ";
                query += "tx_bytes=";
                query += Utils::to_string( tx );
                query += " WHERE row='" + row + "'";
                query += ";";
                rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

                if ( rc != SQLITE_OK )
                {
                    sqlite3_free( zErrMsg );
                    Logger::LogError( "Can not update " + table_name + " with row: " + row );
                    continue;
                }
            }
        }

        sqlite3_close_v2( db );
    }

    Globals::data_load_save_mutex.unlock();

#endif // use_sqlite
}

/** @brief save_stats_to_files
  *
  * The method saves the statistics to the files database
  * The database file is determined based on
  * the current working directory and network interface MAC address.
  *
  * @param mac
  * @return void
  *
  */
void Utils::save_stats_to_files( const string& )
{
}

/** @brief save_stats_to_mysql
  *
  * The method saves the statistics to the mysql database
  * The database file is determined based on
  * the current working directory and network interface MAC address.
  *
  * @param mac
  * @return none
  *
  */
void Utils::save_stats_to_mysql( const string& )
{
#ifdef use_mysql

    MYSQL *conn = mysql_init( NULL );


    for ( const auto & mac_table : all_stats )
    {
        bool res;


        string mac = mac_table.first;
        const map<string, map<string, InterfaceStats> > & table = mac_table.second;

        if ( mysql_real_connect( conn, "127.0.0.1", "root", "password", NULL, 0, NULL, 0 ) != NULL )
        {
            string query( "CREATE DATABASE IF NOT EXISTS `" );
            query.append( mac.c_str() );
            query.append( "`" );
            mysql_query( conn, query.c_str() );
        }
        else
        {
            cout << mysql_error( conn ) << endl;
            mysql_close( conn );
            conn = NULL;
            return;
        }

        mysql_close( conn );

        if ( mysql_real_connect( conn, "127.0.0.1", "root", "password", mac.c_str(), 0, NULL, 0 ) != NULL )
        {
            continue;
        }

        for ( const auto & table_row : table )
        {
            string table_name = table_row.first;

            string query;
            query += "CREATE TABLE IF NOT EXISTS `" + table_name + "` (`row` VARCHAR(45) NULL,`rx_bytes` BIGINT NULL,`tx_bytes` BIGINT NULL,PRIMARY KEY (`row`));";

            mysql_query( conn, query.c_str() );

//			if ( res == false )
//			{
//				continue;
//			}

            const map<string, InterfaceStats> row = table_row.second;

            for ( auto const row_stats : row )
            {
                string row = row_stats.first;
                const InterfaceStats& stats = row_stats.second;
                uint64_t rx = stats.recieved();
                uint64_t tx = stats.transmited();

                string query;
                query += "INSERT OR IGNORE INTO " + table_name + " (row,rx_bytes,tx_bytes) VALUES(";
                query += "'";
                query += row;
                query += "'";
                query += ",";
                query += Utils::to_string( rx );
                query += ",";
                query += Utils::to_string( tx );
                query += ");";

                mysql_query( conn, query.c_str() );

//				if ( res == false )
//				{
//					printf( "error\n" );
//					continue;
//				}


                query.clear();
                query += "UPDATE OR IGNORE " + table_name + " SET ";
                query += "rx_bytes=";
                query += Utils::to_string( rx );
                query += ", ";
                query += "tx_bytes=";
                query += Utils::to_string( tx );
                query += " WHERE row='" + row + "'";
                query += ";";

                mysql_query( conn, query.c_str() );

                if ( res == false )
                {
                    continue;
                }
            }
        }

        mysql_close( conn );
    }

#endif // use_mysql
}

/** @brief check_database_integrity
  *
  * This function returns true if the database integrity is maintained
  * and returns false if database integrity is lost
  *
  * @param mac
  * @return True if database integrity is maintained
  *
  */
bool Utils::check_database_integrity( const string& a_mac )
{
    uint32_t y;
    uint32_t m;
    uint32_t d;
    uint32_t h;
    vector<string> macs;

    Utils::get_time( &y, &m, &d, &h );

    struct date start_date;
    struct date end_date;

    macs.clear();

    if ( a_mac.compare( "" ) == 0 )
    {
        const map<string, InterfaceInfo>& interfaces = Utils::get_all_interfaces();

        for ( auto const & kv : interfaces )
        {
            const InterfaceInfo& in = kv.second;
            const string& mac = in.get_mac();
            macs.push_back( mac );
        }
    }
    else
    {
        macs.push_back( a_mac );
    }

    for ( string const & mac : macs )
    {
        start_date.year = 1900;
        start_date.month = 1;
        start_date.day = 1;
        start_date.hour = 0;

        end_date.year = y;
        end_date.month = m;
        end_date.day = d;
        end_date.hour = h;

        map<string, InterfaceStats> results = Globals::db_drv.get_stats( mac, "hourly", start_date, end_date );

        uint64_t hourly_rx = 0ULL;
        uint64_t hourly_tx = 0ULL;

        for ( auto & row_stats : results )
        {
            const InterfaceStats& stats = row_stats.second;

            hourly_rx += stats.received();
            hourly_tx += stats.transmitted();
        }

        start_date.year = 1900;
        start_date.month = 0;
        start_date.day = 0;
        start_date.hour = 0;

        end_date.year = y;
        end_date.month = 0;
        end_date.day = 0;
        end_date.hour = 0;

        results = Globals::db_drv.get_stats( mac, "yearly", start_date, end_date );

        uint64_t yearly_rx = 0ULL;
        uint64_t yearly_tx = 0ULL;

        for ( auto & row_stats : results )
        {
            const InterfaceStats& stats = row_stats.second;

            yearly_rx += stats.received();
            yearly_tx += stats.transmitted();
        }

        if ( hourly_rx != yearly_rx || hourly_tx != yearly_tx )
        {
            return false;
        }
    }

    return true;
}

/** @brief repair_broken_database
  *
  * This function repairs the broken database
  * and returns true if everything went ok.
  *
  * @param mac address of network interface
  * @return True when database has been repaired
  *
  */
bool Utils::repair_broken_database( const string& a_mac )
{
#ifdef use_sqlite
    uint32_t y;
    uint32_t m;
    uint32_t d;
    uint32_t h;
    sqlite3 *db;
    char *zErrMsg = nullptr;
    vector<string> macs;

    vector<string> queries;

    Utils::get_time( &y, &m, &d, &h );

    struct date start_date;
    struct date end_date;

    macs.clear();

    if ( a_mac.compare( "" ) == 0 )
    {
        const map<string, InterfaceInfo>& interfaces = Utils::get_all_interfaces();

        for ( auto const & kv : interfaces )
        {
            const InterfaceInfo& in = kv.second;
            const string& mac = in.get_mac();
            macs.push_back( mac );
        }
    }
    else
    {
        macs.push_back( a_mac );
    }

    for ( string const & mac : macs )
    {
        queries.clear();

        // fix yearly stats
        start_date.year = 1900;
        start_date.month = 1;
        start_date.day = 1;
        start_date.hour = 0;

        end_date.year = y;
        end_date.month = m;
        end_date.day = d;
        end_date.hour = h;

        map<string, InterfaceStats> results = Globals::db_drv.get_stats( mac, "yearly", start_date, end_date );

        for ( auto & row_stats : results )
        {
            const string& row = row_stats.first;

            struct date hourly_start_date;
            struct date hourly_end_date;

            hourly_start_date.year = Utils::stoi( row );
            hourly_start_date.month = 1;
            hourly_start_date.day = 1;
            hourly_start_date.hour = 0;

            hourly_end_date.year = Utils::stoi( row );
            hourly_end_date.month = 12;
            hourly_end_date.day = 31;
            hourly_end_date.hour = 23;

            map<string, InterfaceStats> hourly_results = Globals::db_drv.get_stats( mac, "hourly", hourly_start_date, hourly_end_date );

            uint64_t total_received = 0ULL;
            uint64_t total_transmitted = 0ULL;

            for ( auto & hourly_row_stats : hourly_results )
            {
                const InterfaceStats& hourly_stats = hourly_row_stats.second;
                total_received += hourly_stats.received();
                total_transmitted += hourly_stats.transmitted();
            }

            string query;
            query.clear();
            query += "UPDATE OR IGNORE yearly SET ";
            query += "rx_bytes=";
            query += Utils::to_string( total_received );
            query += ", ";
            query += "tx_bytes=";
            query += Utils::to_string( total_transmitted );
            query += " WHERE row='" + row + "'";
            query += ";";
            queries.push_back( query );
        }

        // fix monthly stats
        results = Globals::db_drv.get_stats( mac, "monthly", start_date, end_date );

        for ( auto & row_stats : results )
        {
            const string& row = row_stats.first;

            uint32_t current_year, current_month, current_day, current_hour;

            str2date( row, "monthly", &current_year, &current_month, &current_day, &current_hour );

            struct date hourly_start_date;
            struct date hourly_end_date;

            hourly_start_date.year = current_year;
            hourly_start_date.month = current_month;
            hourly_start_date.day = 1;
            hourly_start_date.hour = 0;

            hourly_end_date.year = current_year;
            hourly_end_date.month = current_month;
            hourly_end_date.day = Utils::days_in_month( current_month, current_year );
            hourly_end_date.hour = 23;

            map<string, InterfaceStats> hourly_results = Globals::db_drv.get_stats( mac, "hourly", hourly_start_date, hourly_end_date );

            uint64_t total_received = 0ULL;
            uint64_t total_transmitted = 0ULL;

            for ( auto & hourly_row_stats : hourly_results )
            {
                const InterfaceStats& hourly_stats = hourly_row_stats.second;
                total_received += hourly_stats.received();
                total_transmitted += hourly_stats.transmitted();
            }

            string query;
            query.clear();
            query += "UPDATE OR IGNORE monthly SET ";
            query += "rx_bytes=";
            query += Utils::to_string( total_received );
            query += ", ";
            query += "tx_bytes=";
            query += Utils::to_string( total_transmitted );
            query += " WHERE row='" + row + "'";
            query += ";";
            queries.push_back( query );
        }

        // fix daily stats
        results = Globals::db_drv.get_stats( mac, "daily", start_date, end_date );

        for ( auto & row_stats : results )
        {
            const string& row = row_stats.first;

            uint32_t current_year, current_month, current_day, current_hour;

            str2date( row, "daily", &current_year, &current_month, &current_day, &current_hour );

            struct date hourly_start_date;
            struct date hourly_end_date;

            hourly_start_date.year = current_year;
            hourly_start_date.month = current_month;
            hourly_start_date.day = current_day;
            hourly_start_date.hour = 0;

            hourly_end_date.year = current_year;
            hourly_end_date.month = current_month;
            hourly_end_date.day = current_day;
            hourly_end_date.hour = 23;

            map<string, InterfaceStats> hourly_results = Globals::db_drv.get_stats( mac, "hourly", hourly_start_date, hourly_end_date );

            uint64_t total_received = 0ULL;
            uint64_t total_transmitted = 0ULL;

            for ( auto & hourly_row_stats : hourly_results )
            {
                const InterfaceStats& hourly_stats = hourly_row_stats.second;
                total_received += hourly_stats.received();
                total_transmitted += hourly_stats.transmitted();
            }

            string query;
            query.clear();
            query += "UPDATE OR IGNORE daily SET ";
            query += "rx_bytes=";
            query += Utils::to_string( total_received );
            query += ", ";
            query += "tx_bytes=";
            query += Utils::to_string( total_transmitted );
            query += " WHERE row='" + row + "'";
            query += ";";
            queries.push_back( query );
        }

        Globals::data_load_save_mutex.lock();

        int rc = sqlite3_open_v2( ( Globals::cwd + PATH_SEPARATOR + mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr );

        if ( rc != SQLITE_OK )
        {
            Globals::data_load_save_mutex.unlock();
            continue;
        }

        for ( const string& query : queries )
        {
            rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

            if ( rc != SQLITE_OK )
            {
                Logger::LogError( "Can not fix table with query: " + query );
            }

            sqlite3_db_cacheflush( db );
        }

        sqlite3_close_v2( db );

        Globals::data_load_save_mutex.unlock();
    }

    return true;

#endif // use_sqlite
}

/** @brief callback
  *
  * The callback method used to query database.
  * The method is not used outside the Utils class.
  *
  */
int Utils::callback( void*, int argc, char** argv, char** azColName )
{
#ifdef use_sqlite

    if ( argc > 0 )
    {
        for ( int i = 0; i < argc; i++ )
        {
            Utils::table_columns[string( azColName[i] )] = string( argv[i] ? argv[i] : "0" );
        }

        return 0;
    }
    else
    {
        return 1;
    }

#endif
}

/** @brief stoi
  *
  * The method converts decimal string to 32bit signed integer
  *
  * @param decimal string
  * @return converted value as a 32bit signed integer
  *
  */
int32_t Utils::stoi( const string & str )
{
    if ( str.size() == 0 )
    {
        return 0;
    }

    int32_t end_point;
    bool neg;

    if ( str[0] == '-' )
    {
        neg = true;
        end_point = 1;
    }
    else
    {
        neg = false;
        end_point = 0;
    }

    int32_t result = 0;
    int32_t mul = 1;

    for ( int32_t i = str.size() - 1; i >= end_point; i-- )
    {
        result += ( str[i] - '0' ) * mul;
        mul *= 10;
    }

    if ( neg == true )
    {
        result = -result;
    }

    return result;
}

/** @brief stoui
  *
  * The method converts decimal string to 32bit unsigned integer
  *
  * @param decimal string
  * @return converted value as a 32bit unsigned integer
  *
  */
uint32_t Utils::stoui( const string & str )
{
    if ( str.size() == 0 )
    {
        return 0;
    }

    uint32_t result = 0U;
    uint32_t mul = 1U;

    for ( int32_t i = str.size() - 1; i >= 0; i-- )
    {
        if ( str[i] < '0' || str[i] > '9' )
        {
            return 0U;
        }

        result += ( str[i] - '0' ) * mul;
        mul *= 10U;
    }

    return result;
}

/** @brief stoull
  *
  * The method converts decimal string to 64bit unsigned integer
  *
  * @param decimal string
  * @return converted value as a 64bit unsigned integer
  *
  */
uint64_t Utils::stoull( const string & str )
{
    if ( str.size() == 0 )
    {
        return 0ULL;
    }

    int32_t end_point;
    bool neg;

    if ( str[0] == '-' )
    {
        neg = true;
        end_point = 1;
    }
    else
    {
        neg = false;
        end_point = 0;
    }

    uint64_t result = 0ULL;
    uint64_t mul = 1ULL;

    for ( int32_t i = str.size() - 1; i >= end_point; i-- )
    {
        result += ( str[i] - '0' ) * mul;
        mul *= 10ULL;
    }

    if ( neg == true )
    {
        result = -result;
    }

    return result;
}

/** @brief hstoi
  *
  * The method converts hexadecimal string to integer
  *
  * @param hexadecimal string
  * @return converted value as an integer
  *
  */
int32_t Utils::hstoi( const string & str )
{
    int32_t x;
    stringstream ss;
    ss << std::hex << str;
    ss >> x;

    return x;
}

/** @brief set_signals_handler
  *
  * The method sets signal handlers
  *
  * @param none
  * @return none
  *
  */
void Utils::set_signals_handler( void )
{
#ifdef _WIN32
    WindowsUtils::set_signals_handler();
#endif // _WIN32

#ifdef __linux
    LinuxUtils::set_signals_handler();
#endif // __linux
}

/** @brief BecomeDaemon
  *
  * The method starts the program as a daemon (service)
  *
  * @param none
  * @return daemon status as an integer
  *
  */
int32_t Utils::BecomeDaemon( void )
{
#ifdef _WIN32
    return WindowsUtils::become_daemon();
#endif // _WIN32

#ifdef __linux
    return LinuxUtils::BecomeDaemon( BD_NO_CHDIR | BD_NO_UMASK0 );
#endif // __linux
}

/** @brief make_program_run_at_startup
  *
  * The method makes the program run at computer startup.
  *
  * @param none
  * @return none
  *
  */
void Utils::MeterThread( void )
{
#ifdef _WIN32
    WindowsUtils::MeterThread();
#endif // _WIN32

#ifdef __linux
    LinuxUtils::MeterThread();
#endif // __linux
}

/** @brief make_program_run_at_startup
  *
  * The method makes the program run at computer startup.
  *
  * @param none
  * @return none
  *
  */
void Utils::make_program_run_at_startup( void )
{
#ifdef _WIN32
    WindowsUtils::make_program_run_at_startup();
#endif // _WIN32

#ifdef __linux

#endif // __linux
}

/** @brief get_program_path
  *
  * The method returns the program path.
  *
  * @param programs' arguments
  * @return program path as a string
  *
  */
string Utils::get_program_path( char** argv )
{
#ifdef _WIN32
    return WindowsUtils::get_program_path();
#endif // _WIN32

#ifdef __linux
    return LinuxUtils::get_program_path( argv[0] );
#endif // __linux
}

/** @brief sleep_seconds
  *
  * The method sleeps for a given number of seconds.
  *
  * @param seconds to sleep
  * @return none
  *
  */
string Utils::get_path( const string & full_file_path )
{
    string path = boost::filesystem::path( full_file_path ).parent_path().generic_string();

#ifdef _WIN32
    std::replace( path.begin(), path.end(), '/', PATH_SEPARATOR_CAHR );
#endif // _WIN32

    return path;
}

/** @brief sleep_seconds
  *
  * The method sleeps for a given number of seconds.
  *
  * @param seconds to sleep
  * @return none
  *
  */
void Utils::sleep_seconds( uint32_t seconds )
{
#ifdef _WIN32
    Sleep( 1000 * seconds );
#endif // _WIN32

#ifdef __linux
    sleep( seconds );
#endif // __linux
}

/** @brief set_endsession_handler
  *
  * The method sets endsession handler
  *
  * @param none
  * @return none
  *
  */
void Utils::set_endsession_handler( void )
{
#ifdef __WIN32
    WindowsUtils::handle_endsession_message();
#endif // __WIN32
}

/** @brief dir_exists
  *
  * The method checks if a given path exists.
  *
  * @param directory path to check
  * @return True if path exists, false otherwise
  *
  */
bool Utils::dir_exists( const char* path )
{
#ifdef _WIN32
    return WindowsUtils::dir_exists( path );
#endif // _WIN32

#ifdef __linux
    return LinuxUtils::dir_exists( path );
#endif // __linux
}

/** @brief make_path
  *
  * The method creates a new directory
  *
  * @param directory path to create
  * @param new directory mode
  * @return none
  *
  */
int32_t Utils::make_path( const string & _s, mode_t mode )
{
#ifdef _WIN32
    return WindowsUtils::make_path( path );
#endif // _WIN32

#ifdef __linux
    return LinuxUtils::make_path( _s, mode );
#endif // __linux
}

/** @brief save_pid_file
  *
  * The method saves pid file
  *
  * @param pid file path
  * @return none
  *
  */
void Utils::save_pid_file( const string & pid_file_path )
{
#ifdef __linux
    LinuxUtils::save_pid_file( pid_file_path );
#endif // __linux
}

/** @brief rfc1123_datetime
  *
  * The method converts time_t structure to rfc1123 date format.
  *
  * @param time
  * @return rfc1123 date
  *
  */
string Utils::rfc1123_datetime( time_t time )
{
    struct tm * timeinfo;
    char buffer [80];

    timeinfo = gmtime( &time );
    strftime( buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeinfo );

    return string( buffer );
}

/** @brief save_settings
  *
  * The method saves all settings
  * in the Settings::settings["config directory"] + PATH_SEPARATOR + smarttrafficmeter.conf
  *
  * @param none
  * @return True if save successful
  *
  */
bool Utils::save_settings( void )
{
    ofstream file;
    file.open( Settings::settings["config directory"] + PATH_SEPARATOR + "smarttrafficmeter.conf", std::ofstream::out );

    if ( file.is_open() )
    {
        for ( auto const & settings_item : Settings::settings )
        {
            const string& key = settings_item.first;
            const string& value = settings_item.second;

            file << key << "=" << value << endl;
        }

        file.close();
    }
    else
    {
        return false;
    }

    return true;
}

/** @brief gz_compress
  *
  * The method is used to compress a string using gz compression.
  *
  * @param data for the compression as a string
  * @return compressed data as a string
  *
  */
string Utils::gz_compress( const std::string& data )
{
    namespace bio = boost::iostreams;

    std::stringstream compressed;
    std::stringstream origin( data );

    bio::filtering_streambuf<bio::input> out;
    out.push( bio::gzip_compressor( bio::gzip_params( bio::gzip::best_compression ) ) );
    out.push( origin );
    bio::copy( out, compressed );

    return compressed.str();
}

/** @brief ensure_dir
  *
  * The method is used to create a new directory if it does not exists
  *
  * @param new directory as a string
  * @return creation status, 0 on success
  *
  */
int32_t Utils::ensure_dir( const string& dir )
{
#ifdef __linux
    return LinuxUtils::ensure_dir( dir );
#endif // __linux
}

/** @brief launch_default_browser
  *
  * This method opens the given url in a default browser
  *
  * @param the url to be opened
  * @return The value returned is -1 on error and the return status of the command otherwise.
  *
  */
int32_t Utils::launch_default_browser(const string& url )
{
#ifdef __linux
    return LinuxUtils::launch_default_browser( url );
#endif // __linux
}


