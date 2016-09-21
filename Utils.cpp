#include "config.h"
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <boost/filesystem.hpp>

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


map<string, string> Utils::table_columns;


map<string, InterfaceInfo> Utils::get_all_interfaces( void )
{
#ifdef _WIN32
    return WindowsUtils::get_all_interfaces();
#endif // _WIN32

#ifdef __linux
    return LinuxUtils::get_all_interfaces();
#endif // __linux
}

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
  * @todo: document this function
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
  * @todo: document this function
  */
void Utils::get_os_info(string& os_info)
{
	#ifdef _WIN32

#endif // _WIN32

#ifdef __linux
    return LinuxUtils::get_os_info( os_info );
#endif // __linux
}


void Utils::get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h )
{
    time_t t = time( nullptr );
    struct tm* tm = localtime( &t );
    ( *y ) = tm->tm_year + 1900;
    ( *m ) = tm->tm_mon + 1;
    ( *d ) = tm->tm_mday;
    ( *h ) = tm->tm_hour;
}

/** @brief get_time_from_milisec
  *
  * @todo: document this function
  */
void Utils::get_time_from_milisec( time_t t, uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h )
{
    struct tm* tm = localtime( &t );
    ( *y ) = tm->tm_year + 1900;
    ( *m ) = tm->tm_mon + 1;
    ( *d ) = tm->tm_mday;
    ( *h ) = tm->tm_hour;
}


/** @brief contians
  *
  * the function returns true if key is found in the str
  *
  */
bool Utils::contians( const string& str, const string& key )
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
  *function returns true if str starts with key
  *
  */
bool Utils::starts_with( const string& str, const string& key )
{
    if ( str.find( key ) == 0 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/** @brief split
  *
  * @todo: document this function
  */
vector<string> Utils::split( const string& str, const string& delim )
{
//http://garajeando.blogspot.com/2014/03/using-c11-to-split-strings-without.html
//http://www.informit.com/articles/article.aspx?p=2064649&seqNum=
//http://stackoverflow.com/questions/236129/split-a-string-in-c

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
  * @todo: document this function
  */
string Utils::replace( const string& pattern, const string& with, const string& in ) noexcept
{

    string out;

    size_t start = 0;
    size_t end = 0;

    if ( in.find( pattern, start ) == string::npos )
    {
        out += in;
    }
    else
    {
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
/** @brief to_string
  *
  * @todo: document this function
  */
string Utils::float_to_string( float value, int32_t precision )
{
    std::ostringstream stm ;
    stm.precision( precision );
    stm << value ;
    return stm.str() ;
}

/** @brief to_string
  *
  * @todo: document this function
  */
string Utils::double_to_string( double value, int32_t precision )
{
    std::ostringstream stm ;
    stm.precision( precision );
    stm << value ;
    return stm.str() ;
}

/** @brief to_string
  *
  * @todo: document this function
  */
string Utils::to_string( uint64_t value, uint32_t min_string_lenght )
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

    for ( int32_t i = 0; i < ( ( int32_t ) min_string_lenght - ( int32_t ) out.size() ); i++ )
    {
        out.insert( 0, "0" );
    }

    return out;
}


/** @brief to_string
  *
  * @todo: document this function
  */
string Utils::to_string( uint32_t value, uint32_t min_string_lenght )
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

    for ( int32_t i = 0; i < ( ( int32_t ) min_string_lenght - ( int32_t ) out.size() ); i++ )
    {
        out.insert( 0, "0" );
    }

    return out;
}

/** @brief stof
  *
  * @todo: document this function
  */
float Utils::stof( const string& str )
{
    float value;

    std::stringstream stream( str );

    stream >> value;

    if ( stream.fail() )
    {
        return 0.0f;
    }

    return value;
}


/** @brief hexcolor_to_strings
  *
  * @todo: document this function
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
  * The function returns true if the current instance is the only one
  * instance running. Otherwise, the function returns false.
  * @return bool
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

/** @brief trim
  *
  * @todo: document this function
  */
string Utils::trim( const std::string& s )
{
    //http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

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
  * @todo: document this function
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
/** @brief load_data_from_sqlite
  *
  * @todo: document this function
  */
void Utils::load_data_from_sqlite( void )
{
#ifdef use_sqlite

    Globals::data_load_save_mutex.lock();

    sqlite3 *db;
    char *zErrMsg = nullptr;
    int rc;
    uint32_t y;
    uint32_t m;
    uint32_t d;
    uint32_t h;
    uint64_t rx_bytes;
    uint64_t tx_bytes;

    Utils::get_time( &y, &m, &d, &h );

    const map<string, InterfaceInfo>& interfaces = Utils::get_all_interfaces();

    string row;
    string query;

    for ( auto const & kv : interfaces )
    {
        const InterfaceInfo& in = kv.second;
        const string& mac = in.get_mac();

        rc = sqlite3_open_v2( ( Globals::cwd + PATH_SEPARATOR + mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE, nullptr );

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

        sqlite3_close( db );
    }

    Globals::data_load_save_mutex.unlock();

#endif // use_sqlite
}


/** @brief load_data_from_files
  *
  * @todo: document this function
  */
void Utils::load_data_from_files( void )
{
//    uint32_t y;
//    uint32_t m;
//    uint32_t d;
//    uint32_t h;
//    uint64_t rx_bytes;
//    uint64_t tx_bytes;
//    Globals::data_load_save_mutex.lock();
//
//    Utils::get_time( &y, &m, &d, &h );
//
//    const map<string, InterfaceInfo>& interfaces = Utils::get_all_interfaces();
//
//    string row;
//    ifstream file;
//
//    for ( auto const & kv : interfaces )
//    {
//        const InterfaceInfo& in = kv.second;
//        const string& mac = in.get_mac();
//
/////
//        row.clear();
//        row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";
//        file.open( Globals::cwd + PATH_SEPARATOR + mac + PATH_SEPARATOR + "hourly" PATH_SEPARATOR+ row + PATH_SEPARATOR + "stats.txt", std::ifstream::in );
//
//        if ( file.is_open() == true )
//        {
//            file >> rx_bytes;
//            file >> tx_bytes;
//            file.close();
//            Globals::all_stats[mac]["hourly"][row].set_initial_stats( tx_bytes, rx_bytes );
//        }
//
/////
//        row.clear();
//        row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );
//        file.open( cwd + "/" + mac + "/daily/" + row + "/stats.txt", ifstream::in );
//
//        if ( file.is_open() == true )
//        {
//            file >> rx_bytes;
//            file >> tx_bytes;
//            file.close();
//            Globals::all_stats[mac]["daily"][row].set_initial_stats( tx_bytes, rx_bytes );
//        }
//
/////
//        row.clear();
//        row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 );
//        file.open( cwd + "/" + mac + "/monthly/" + row + "/stats.txt", ifstream::in );
//
//        if ( file.is_open() == true )
//        {
//            file >> rx_bytes;
//            file >> tx_bytes;
//            file.close();
//            Globals::all_stats[mac]["monthly"][row].set_initial_stats( tx_bytes, rx_bytes );
//        }
//
/////
//        row.clear();
//        row += Utils::to_string( y );
//        file.open( cwd + "/" + mac + "/yearly/" + row + "/stats.txt", ifstream::in );
//
//        if ( file.is_open() == true )
//        {
//            file >> rx_bytes;
//            file >> tx_bytes;
//            file.close();
//            Globals::all_stats[mac]["yearly"][row].set_initial_stats( tx_bytes, rx_bytes );
//        }
//    }
//  Globals::data_load_save_mutex.unlock();
}

/** @brief save_stats_to_sqlite
  *
  * @todo: document this function
  */
void Utils::save_stats_to_sqlite( void )
{
#ifdef use_sqlite
//http://stackoverflow.com/questions/18794580/mysql-create-table-if-not-exists-in-phpmyadmin-import
//http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm
//https://www.sqlite.org/cintro.html

    Globals::data_load_save_mutex.lock();

    string query;

    for ( auto const & mac_table : Globals::all_stats )
    {
        sqlite3 *db;
        char *zErrMsg = nullptr;
        int rc;

        const string& mac = mac_table.first;
        const map<string, map<string, InterfaceStats> > & table = mac_table.second;

        rc = sqlite3_open_v2( ( Globals::cwd + PATH_SEPARATOR + mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr );

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


            query.clear();
            query += "DELETE FROM " + table_name + " WHERE ";
            query += "row NOT LIKE ";
            query += "'2%%';";
            rc = sqlite3_exec( db, query.c_str(), callback, nullptr, &zErrMsg );

            if ( rc != SQLITE_OK )
            {
                sqlite3_free( zErrMsg );
                Logger::LogError( "Can not delete empty row in the table " + table_name );
            }

            const map<string, InterfaceStats> & row = table_row.second;

            for ( auto const & row_stats : row )
            {
                const string& row = row_stats.first;
                const InterfaceStats& stats = row_stats.second;
                uint64_t rx = stats.recieved();
                uint64_t tx = stats.transmited();

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

//        string query;
//
//        query.clear();
//        query+="DELETE FROM 'hourly' WHERE rx_bytes=\"0\" AND tx_bytes=\"0\";";
//        sqlite3_exec( db, query.c_str(), Utils::callback, nullptr, &zErrMsg );
//
//        query.clear();
//        query+="DELETE FROM 'daily' WHERE rx_bytes=\"0\" AND tx_bytes=\"0\";";
//        sqlite3_exec( db, query.c_str(), Utils::callback, nullptr, &zErrMsg );
//
//        query.clear();
//        query+="DELETE FROM 'monthly' WHERE rx_bytes=\"0\" AND tx_bytes=\"0\";";
//        sqlite3_exec( db, query.c_str(), Utils::callback, nullptr, &zErrMsg );
//
//        query.clear();
//        query+="DELETE FROM 'yearly' WHERE rx_bytes=\"0\" AND tx_bytes=\"0\";";
//        sqlite3_exec( db, query.c_str(), Utils::callback, nullptr, &zErrMsg );

        sqlite3_close_v2( db );
    }

    Globals::data_load_save_mutex.unlock();

#endif // use_sqlite
}

/** @brief save_stats_to_files
  *
  * @todo: document this function
  */
void Utils::save_stats_to_files( void )
{
//  Globals::data_load_save_mutex.lock();
//    for ( auto const & mac_table : Globals::all_stats )
//    {
//        const string& mac = mac_table.first;
//        mkpath( mac, 0755 );
//
//        const map<string, map<string, InterfaceStats> > & table = mac_table.second;
//
//        for ( auto const & table_row : table )
//        {
//            const string& table_name = table_row.first;
//            mkpath( mac + "/" + table_name, 0755 );
//
//            const map<string, InterfaceStats> & row = table_row.second;
//
//            for ( auto const & row_stats : row )
//            {
//                const string& row = row_stats.first;
//                mkpath( mac + "/" + table_name + "/" + row, 0755 );
//
//                const InterfaceStats& stats = row_stats.second;
//
//                ofstream file;
//                file.open( cwd + "/" + mac + "/" + table_name + "/" + row + "/stats.txt" );
//
//                if ( file.is_open() == true )
//                {
//                    uint64_t rx = stats.recieved();
//                    uint64_t tx = stats.transmited();
//
//                    file << ( Utils::to_string( rx ) );
//                    file << endl;
//                    file << ( Utils::to_string( tx ) );
//
//                    file.close();
//                }
//            }
//        }
//    }
//  Globals::data_load_save_mutex.unlock();
}

/** @brief callback
  *
  * @todo: document this function
  */
int Utils::callback( void*, int argc, char** argv, char** azColName )
{
#ifdef use_sqlite
    int i;

    if ( argc > 0 )
    {
        for ( i = 0; i < argc; i++ )
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
  * @todo: document this function
  */
int32_t Utils::stoi( const string& str )
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


/** @brief stoull
  *
  * @todo: document this function
  */
uint64_t Utils::stoull( const string& str )
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
  * @todo: document this function
  */
int32_t Utils::hstoi( const string& str )
{
    int32_t x;
    stringstream ss;
    ss << std::hex << str;
    ss >> x;

    return x;
}

/** @brief set_signals_handler
  *
  * @todo: document this function
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
  * @todo: document this function
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

/** @brief MeterThread
  *
  * @todo: document this function
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
  * @todo: document this function
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
  * @todo: document this function
  */
string Utils::get_program_path( char** argv )
{
#ifdef _WIN32
    return WindowsUtils::get_program_path();
#endif // _WIN32

#ifdef __linux
    return LinuxUtils::get_get_program_path( argv[0] );
#endif // __linux
}


/** @brief get_path
  *
  * @todo: document this function
  */
string Utils::get_path( const string& full_file_path )
{
    string path = boost::filesystem::path( full_file_path ).parent_path().generic_string();

#ifdef _WIN32
    std::replace( path.begin(), path.end(), '/', PATH_SEPARATOR_CAHR );
#endif // _WIN32

    return path;
}
/** @brief sleep
  *
  * @todo: document this function
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

string Utils::to_narrow( const wchar_t* src )
{
    size_t i;
    wchar_t code;
    string out;

    i = 0;

    while ( src[i] != '\0' )
    {
        code = src[i];

        if ( code < 128 )
        {
            out += char ( code );
        }
        else
        {
            out += ' ';

            // if lead surrogate, skip the next code unit, which is the trail
            if ( code >= 0xD800 && code <= 0xD8FF )
            {
                i++;
            }
        }

        i++;
    }

    return out;
}
void Utils::set_endsession_handler( void )
{
#ifdef __WIN32
    WindowsUtils::handle_endsession_message();
#endif // __WIN32
}



//https://github.com/fmtlib/fmt

