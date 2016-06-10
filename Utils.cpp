#include "config.h"
#include <algorithm>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <stdio.h>
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

#ifdef use_sqlite
#include "sqlite3.h"
#endif // use_sqlite

#include "Utils.h"
#include "Logger.h"
#include "Globals.h"


map<string, string> Utils::table_columns;


map<string, InterfaceInfo> Utils::get_all_interfaces( void )
{
    map<string, InterfaceInfo> interfaces;

    struct ifaddrs *ifaddr, *ipa = nullptr;
    int family, s;
    char* host = new char[NI_MAXHOST];

    if ( getifaddrs( &ifaddr ) == -1 )
    {
        freeifaddrs( ifaddr );
        return interfaces;
    }

    ipa = ifaddr;

    for ( ; ifaddr != NULL; ifaddr = ifaddr->ifa_next )
    {
        if ( ifaddr->ifa_addr == NULL || strcmp( ifaddr->ifa_name, "lo" ) == 0 )
        {
            continue;
        }

        family = ifaddr->ifa_addr->sa_family;


        /* For an AF_INET* interface address, display the address */

        if ( family == AF_INET )
        {
            s = getnameinfo( ifaddr->ifa_addr,
                             ( family == AF_INET ) ? sizeof( struct sockaddr_in ) :
                             sizeof( struct sockaddr_in6 ),
                             host, NI_MAXHOST,
                             NULL, 0U, NI_NUMERICHOST );

            if ( s != 0 )
            {
                continue;
            }

            string interface_name( ifaddr->ifa_name );

            if ( interfaces.find( interface_name ) == interfaces.end() )
            {
                InterfaceInfo in;
                in.set_name( ifaddr->ifa_name );
                in.set_mac( get_mac( ifaddr->ifa_name ).c_str() );
                in.set_ip4( host );
                interfaces[interface_name] = in;
            }
            else
            {
                interfaces[interface_name].set_ip4( host );
            }
        }
        else if ( family == AF_INET6 )
        {
            s = getnameinfo( ifaddr->ifa_addr,
                             ( family == AF_INET ) ? sizeof( struct sockaddr_in ) :
                             sizeof( struct sockaddr_in6 ),
                             host, NI_MAXHOST,
                             NULL, 0U, NI_NUMERICHOST );

            if ( s != 0 )
            {
                continue;
            }

            string interface_name = string( ifaddr->ifa_name );

            if ( interfaces.find( interface_name ) == interfaces.end() )
            {
                InterfaceInfo in;
                in.set_name( ifaddr->ifa_name );
                in.set_mac( get_mac( ifaddr->ifa_name ).c_str() );
                in.set_ip6( host );
                interfaces[interface_name] = in;
            }
            else
            {
                interfaces[interface_name].set_ip6( host );
            }
        }
    }

    delete []host;

    freeifaddrs( ipa );

    return interfaces;
}

string Utils::get_mac( char* name )
{
    int s;
    struct ifreq buffer;
    char* out_buf;
    string mac;

    out_buf = new char[256];
    memset( out_buf, 0x00, 256 * sizeof( char ) );

    s = socket( PF_INET, SOCK_DGRAM, 0 );

    memset( &buffer, 0x00, sizeof( buffer ) );

    strcpy( buffer.ifr_name, name );

    ioctl( s, SIOCGIFHWADDR, &buffer );

    close( s );

    for ( s = 0; s < 6; s++ )
    {
        sprintf( out_buf, "%.2x-%.2x-%.2x-%.2x-%.2x-%.2x", ( unsigned char ) buffer.ifr_hwaddr.sa_data[0], ( unsigned char ) buffer.ifr_hwaddr.sa_data[1], ( unsigned char ) buffer.ifr_hwaddr.sa_data[2], ( unsigned char ) buffer.ifr_hwaddr.sa_data[3], ( unsigned char ) buffer.ifr_hwaddr.sa_data[4], ( unsigned char ) buffer.ifr_hwaddr.sa_data[5] );
    }

    mac.clear();
    mac.append( out_buf );

    delete []out_buf;

    return mac;
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
                out += std::to_string( c );
            }
        }
    }

    for ( int32_t i = 0; i < ( ( int32_t )min_string_lenght - ( int32_t )out.size() ); i++ )
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
                out += std::to_string( c );
            }
        }
    }

    for ( int32_t i = 0; i < ( ( int32_t )min_string_lenght - ( int32_t )out.size() ); i++ )
    {
        out.insert( 0, "0" );
    }

    return out;
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

    color = std::stoi( color_item_str, nullptr, 16 );

    out.push_back( Utils::to_string( color ) );

    // g
    color_item_str = hex_color.substr( 2, 2 );

    color = std::stoi( color_item_str, nullptr, 16 );

    out.push_back( Utils::to_string( color ) );

    // b
    color_item_str = hex_color.substr( 4, 2 );

    color = std::stoi( color_item_str, nullptr, 16 );

    out.push_back( Utils::to_string( color ) );

    return out;
}



/** @brief check_one_instance
  *
  * @todo: document this function
  */
bool Utils::check_one_instance( void )
{
    int pid_file = open( "smarttrafficmeter.pid", O_CREAT | O_RDWR, 0666 );
    int rc = flock( pid_file, LOCK_EX | LOCK_NB );

    if ( rc != 0 && ( errno == EWOULDBLOCK ) )
    {
        return false;
    }

    return true;
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
void Utils::load_data_from_sqlite(void)
{
#ifdef use_sqlite
    sqlite3 *db;
    char *zErrMsg = 0;
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

        rc = sqlite3_open_v2( ( Globals::cwd + PATH_SEPARATOR + mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE, NULL );

        if ( rc != SQLITE_OK )
        {
            continue;
        }

        query.clear();
        query += "DELETE FROM yearly WHERE row NOT LIKE '2%%';";
        rc = sqlite3_exec( db, query.c_str(), callback, 0, &zErrMsg );

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
        rc = sqlite3_exec( db, query.c_str(), callback, NULL, &zErrMsg );

        if ( rc == SQLITE_OK )
        {
            row = table_columns["row"];

            try
            {
                rx_bytes = std::stoull( table_columns["rx_bytes"] );
            }
            catch ( ... )
            {
                rx_bytes = 0ULL;
            }

            try
            {
                tx_bytes = std::stoull( table_columns["tx_bytes"] );
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
        rc = sqlite3_exec( db, query.c_str(), callback, 0, &zErrMsg );

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
        rc = sqlite3_exec( db, query.c_str(), callback, NULL, &zErrMsg );

        if ( rc == SQLITE_OK )
        {
            row = table_columns["row"];

            try
            {
                rx_bytes = std::stoull( table_columns["rx_bytes"] );
            }
            catch ( ... )
            {
                rx_bytes = 0ULL;
            }

            try
            {
                tx_bytes = std::stoull( table_columns["tx_bytes"] );
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
        rc = sqlite3_exec( db, query.c_str(), callback, 0, &zErrMsg );

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
        rc = sqlite3_exec( db, query.c_str(), callback, NULL, &zErrMsg );

        if ( rc == SQLITE_OK )
        {
            row = table_columns["row"];

            try
            {
                rx_bytes = std::stoull( table_columns["rx_bytes"] );
            }
            catch ( ... )
            {
                rx_bytes = 0ULL;
            }

            try
            {
                tx_bytes = std::stoull( table_columns["tx_bytes"] );
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
        rc = sqlite3_exec( db, query.c_str(), callback, 0, &zErrMsg );

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
        rc = sqlite3_exec( db, query.c_str(), callback, NULL, &zErrMsg );

        if ( rc == SQLITE_OK )
        {
            row = table_columns["row"];

            try
            {
                rx_bytes = std::stoull( table_columns["rx_bytes"] );
            }
            catch ( ... )
            {
                rx_bytes = 0ULL;
            }

            try
            {
                tx_bytes = std::stoull( table_columns["tx_bytes"] );
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

#endif // use_sqlite
}




/** @brief load_data_from_files
  *
  * @todo: document this function
  */
void Utils::load_data_from_files(void)
{
//    uint32_t y;
//    uint32_t m;
//    uint32_t d;
//    uint32_t h;
//    uint64_t rx_bytes;
//    uint64_t tx_bytes;
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
}

/** @brief save_stats_to_sqlite
  *
  * @todo: document this function
  */
void Utils::save_stats_to_sqlite(void)
{
	#ifdef use_sqlite
//http://stackoverflow.com/questions/18794580/mysql-create-table-if-not-exists-in-phpmyadmin-import
//http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm
//https://www.sqlite.org/cintro.html

    string query;

    for ( auto const & mac_table : Globals::all_stats )
    {
        sqlite3 *db;
        char *zErrMsg = 0;
        int rc;

        const string& mac = mac_table.first;
        const map<string, map<string, InterfaceStats> > & table = mac_table.second;

        rc = sqlite3_open_v2( ( Globals::cwd + PATH_SEPARATOR + mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );

        if ( rc != SQLITE_OK )
        {
            continue;
        }

        for ( auto const & table_row : table )
        {
            const string& table_name = table_row.first;

            query.clear();
            query += "CREATE TABLE IF NOT EXISTS '" + table_name + "' ('row' VARCHAR(45) NULL,'rx_bytes' UNSIGNED BIG INT NULL,'tx_bytes' UNSIGNED BIG INT NULL,PRIMARY KEY ('row'));";

            rc = sqlite3_exec( db, query.c_str(), Utils::callback, 0, &zErrMsg );

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
            rc = sqlite3_exec( db, query.c_str(), callback, 0, &zErrMsg );

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
                rc = sqlite3_exec( db, query.c_str(), callback, 0, &zErrMsg );

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
                rc = sqlite3_exec( db, query.c_str(), callback, 0, &zErrMsg );

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

#endif // use_sqlite
}

/** @brief save_stats_to_files
  *
  * @todo: document this function
  */
void Utils::save_stats_to_files(void)
{
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
}

/** @brief callback
  *
  * @todo: document this function
  */
int Utils::callback(void*, int argc, char** argv, char** azColName)
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

