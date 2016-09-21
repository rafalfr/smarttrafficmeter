#ifdef __linux
#include "config.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <map>
#include <csignal>
#include <execinfo.h>
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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/utsname.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>


#include "Utils.h"
#include "Logger.h"
#include "Debug.h"
#include "Globals.h"
#include "Settings.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"

#include "LinuxUtils.h"

#ifndef __pi__

bfd* LinuxUtils::abfd = nullptr;
asymbol ** LinuxUtils::syms = nullptr;
asection* LinuxUtils::text = nullptr;

#endif // __pi__


using namespace std;

/** @brief MeterThread
  *
  * @todo: document this function
  */
void* LinuxUtils::MeterThread( void )
{
    uint32_t y = 0;
    uint32_t m = 0;
    uint32_t d = 0;
    uint32_t h = 0;
    uint32_t ph = 0;
    static uint64_t p_time = 0;
    struct ifaddrs *ifaddr, *ipa = nullptr;
    int family, s;
    char host[NI_MAXHOST];
    uint32_t refresh_interval;	//statistics refresh interval in seconds
    uint32_t save_interval; 	//save interval in seconds
    const string hourly( "hourly" );
    const string daily( "daily" );
    const string monthly( "monthly" );
    const string yearly( "yearly" );


    try
    {
        refresh_interval = std::stoi( Settings::settings["stats refresh interval"] );
    }
    catch ( ... )
    {
        refresh_interval = 1;
    }

    try
    {
        save_interval = std::stoi( Settings::settings["stats save interval"] );
    }
    catch ( ... )
    {
        save_interval = 30 * 60;
    }

    string row;
    string current_row;
    vector<map<string, InterfaceStats>::const_iterator> rows4remove;

    while ( true )
    {

        if ( getifaddrs( &ifaddr ) == -1 )
        {
            freeifaddrs( ifaddr );
            continue;
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

            if ( family == AF_INET || family == AF_INET6 )
            {
                s = getnameinfo( ifaddr->ifa_addr,
                                 ( family == AF_INET ) ? sizeof( struct sockaddr_in ) :
                                 sizeof( struct sockaddr_in6 ),
                                 host, NI_MAXHOST,
                                 nullptr, 0, NI_NUMERICHOST );

                if ( s != 0 )
                {
                    continue;
                }

            }
            else if ( family == AF_PACKET && ifaddr->ifa_data != NULL )
            {
                struct rtnl_link_stats *stats = ( rtnl_link_stats * ) ifaddr->ifa_data;


                Utils::get_time( &y, &m, &d, &h );

                //TODO remove possible memory leak
                const string& mac = Utils::get_mac( ifaddr->ifa_name ).c_str();

                if ( Globals::speed_stats.find( mac ) == Globals::speed_stats.end() )
                {
                    ///TODO load data from database
                    InterfaceSpeedMeter ism;
                    Globals::speed_stats[mac] = ism;
                }

                row.clear();
                row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";

                if ( Globals::all_stats[mac]["hourly"].find( row ) == Globals::all_stats[mac]["hourly"].end() )
                {
                    InterfaceStats hstats;
                    Globals::all_stats[mac]["hourly"][row] = hstats;
                }

                Globals::all_stats[mac]["hourly"][row].update( stats->tx_bytes, stats->rx_bytes );


                row.clear();
                row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );

                if ( Globals::all_stats[mac]["daily"].find( row ) == Globals::all_stats[mac]["daily"].end() )
                {
                    InterfaceStats dstats;
                    Globals::all_stats[mac]["daily"][row] = dstats;
                }

                Globals::all_stats[mac]["daily"][row].update( stats->tx_bytes, stats->rx_bytes );

                row.clear();
                row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 );

                if ( Globals::all_stats[mac]["monthly"].find( row ) == Globals::all_stats[mac]["monthly"].end() )
                {
                    InterfaceStats mstats;
                    Globals::all_stats[mac]["monthly"][row] = mstats;
                }

                Globals::all_stats[mac]["monthly"][row].update( stats->tx_bytes, stats->rx_bytes );


                row.clear();
                row += Utils::to_string( y );

                if ( Globals::all_stats[mac]["yearly"].find( row ) == Globals::all_stats[mac]["yearly"].end() )
                {
                    InterfaceStats ystats;
                    Globals::all_stats[mac]["yearly"][row] = ystats;
                }

                Globals::all_stats[mac]["yearly"][row].update( stats->tx_bytes, stats->rx_bytes );

                Globals::speed_stats[mac].update( stats->rx_bytes, stats->tx_bytes );

                if ( Globals::is_daemon == false )
                {
                    for ( auto const & mac_speedinfo : Globals::speed_stats )
                    {
                        const string& mac = mac_speedinfo.first;
                        const InterfaceSpeedMeter& ism = mac_speedinfo.second;

                        cout << mac << endl;

                        double speed = ( double )ism.get_tx_speed();

                        if ( speed < 1000.0 )
                        {
                            cout << "up: " << speed << " b/s\t";
                        }
                        else if ( speed >= 1000.0 && speed < 1000.0 * 1000.0 )
                        {
                            speed /= 1000.0;
                            cout << "up: " << speed << " Kb/s\t";
                        }
                        else if ( speed >= 1000.0 * 1000.0 && speed < 1000.0 * 1000.0 * 1000.0 )
                        {
                            speed /= 1000.0 * 1000.0;
                            cout << "up: " << speed << " Mb/s\t";
                        }
                        else if ( speed >= 1000.0 * 1000.0 * 1000.0 && speed < 1000.0 * 1000.0 * 1000.0 * 1000.0 )
                        {
                            speed /= 1000.0 * 1000.0 * 1000.0;
                            cout << "up: " << speed << " Gb/s\t";
                        }

                        speed = ( double )ism.get_rx_speed();

                        if ( speed < 1000.0 )
                        {
                            cout << "down: " << speed << " b/s" << endl;
                        }
                        else if ( speed >= 1000.0 && speed < 1000.0 * 1000.0 )
                        {
                            speed /= 1000.0;
                            cout << "down: " << speed << " Kb/s" << endl;
                        }
                        else if ( speed >= 1000.0 * 1000.0 && speed < 1000.0 * 1000.0 * 1000.0 )
                        {
                            speed /= 1000.0 * 1000.0;
                            cout << "down: " << speed << " Mb/s" << endl;
                        }
                        else if ( speed >= 1000.0 * 1000.0 * 1000.0 && speed < 1000.0 * 1000.0 * 1000.0 * 1000.0 )
                        {
                            speed /= 1000.0 * 1000.0 * 1000.0;
                            cout << "down: " << speed << " Gb/s" << endl;
                        }

                        cout << endl;
                    }

                    cout << "-------------------------------" << endl;
                }
            }
        }

        freeifaddrs( ipa );
        //cout << "\033[2J\033[1;1H";

        struct timeval te;
        gettimeofday( &te, nullptr );
        uint64_t c_time = te.tv_sec * 1000ULL + te.tv_usec / 1000ULL;

        if ( c_time >= p_time + ( 1000ULL * save_interval ) || ( h != ph ) )
        {
            const string& storage = Settings::settings["storage"];

            if ( Utils::contians( storage, "mysql" ) )
            {
#ifdef use_mysql
                //save_stats_to_mysql();
#endif // use_mysql
            }

            if ( Utils::contians( storage, "sqlite" ) )
            {
#ifdef use_sqlite
                Utils::save_stats_to_sqlite();
#endif // use_sqlite
            }

            if ( Utils::contians( storage, "files" ) )
            {
                Utils::save_stats_to_files();
            }

            /* remove unused rows from the all_stats container */

            for ( auto const & mac_table : Globals::all_stats )
            {
                const string& mac = mac_table.first;

                const map<string, map<string, InterfaceStats> > & table = mac_table.second;

                for ( auto const & table_row : table )
                {
                    const string& table_name = table_row.first;	//hourly, daily, etc..

                    current_row.clear();

                    if ( table_name.compare( "hourly" ) == 0 )
                    {
                        current_row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";
                    }
                    else if ( table_name.compare( "daily" ) == 0 )
                    {
                        current_row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );
                    }
                    else if ( table_name.compare( "monthly" ) == 0 )
                    {
                        current_row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 );
                    }
                    else if ( table_name.compare( "yearly" ) == 0 )
                    {
                        current_row += Utils::to_string( y );
                    }

                    const map<string, InterfaceStats> & row = table_row.second;

                    rows4remove.clear();

                    for ( map<string, InterfaceStats>::const_iterator it = row.cbegin(); it != row.cend(); )
                    {
                        const string& row_in_table = it->first;	//subsequent rows in the current table

                        if ( row_in_table.compare( current_row ) != 0 )
                        {
                            rows4remove.push_back( it );
                        }

                        it++;
                    }

                    for ( uint32_t i = 0; i < rows4remove.size(); i++ )
                    {
                        Globals::all_stats[mac][table_name].erase( rows4remove[i] );
                    }

                    rows4remove.clear();
                }
            }

            p_time = c_time;
            ph = h;
        }

        sleep( refresh_interval );
    }

    return nullptr;
}

/** @brief signal_handler
  *
  * @todo: document this function
  */
void LinuxUtils::signal_handler( int signal )
{
    const string& storage = Settings::settings["storage"];

    if ( Utils::contians( storage, "mysql" ) )
    {
#ifdef use_mysql
        //save_stats_to_mysql();
#endif // use_mysql
    }

    if ( Utils::contians( storage, "sqlite" ) )
    {
#ifdef use_sqlite
        Utils::save_stats_to_sqlite();
#endif // use_sqlite
    }

    if ( Utils::contians( storage, "files" ) )
    {
        Utils::save_stats_to_files();
    }

    if ( signal == SIGINT )
    {
        Logger::LogInfo( "Process exited as a result of SIGINT" );
    }
    else if ( signal == SIGSEGV )
    {
        const string& backtrace = Debug::get_backtrace();
        Logger::LogError( backtrace );
        Logger::LogError( "Process exited as a result of SIGSEGV" );
    }
    else if ( signal == SIGTERM )
    {
        Logger::LogInfo( "Process exited as a result of SIGTERM" );
    }
    else if ( signal == SIGABRT )
    {
        const string& backtrace = Debug::get_backtrace();
        Logger::LogError( backtrace );
        Logger::LogInfo( "Process exited as a result of SIGABRT" );
    }
    else if ( signal == SIGILL )
    {
        const string& backtrace = Debug::get_backtrace();
        Logger::LogError( backtrace );
        Logger::LogInfo( "Process exited as a result of SIGILL" );
    }
    else if ( signal == SIGFPE )
    {
        const string& backtrace = Debug::get_backtrace();
        Logger::LogError( backtrace );
        Logger::LogInfo( "Process exited as a result of SIGFPE" );
    }

    Globals::shared_mem.get()->remove( "SmartTrafficMeterSharedMemory" );

    exit( 0 );
}

/** @brief BecomeDaemon
  *
  * @todo: document this function
  */
int LinuxUtils::BecomeDaemon( int flags )
{
    int maxfd, fd;

    switch ( fork() )                   /* Become background process */
    {
    case -1:
        return -1;

    case 0:
        break;                     /* Child falls through... */

    default:
        _exit( EXIT_SUCCESS );     /* while parent terminates */
    }

    if ( setsid() == -1 )               /* Become leader of new session */
        return -1;

    switch ( fork() )                   /* Ensure we are not session leader */
    {
    case -1:
        return -1;

    case 0:
        break;

    default:
        _exit( EXIT_SUCCESS );
    }

    if ( !( flags & BD_NO_UMASK0 ) )
        umask( 0 );                     /* Clear file mode creation mask */

    if ( !( flags & BD_NO_CHDIR ) )
    {
        if ( chdir( "/" ) != 0 )
        {
            cout << "can't change working directory" << endl;
        }
    }

    if ( !( flags & BD_NO_CLOSE_FILES ) ) /* Close all open files */
    {
        maxfd = sysconf( _SC_OPEN_MAX );

        if ( maxfd == -1 )              /* Limit is indeterminate... */
            maxfd = BD_MAX_CLOSE;       /* so take a guess */

        for ( fd = 0; fd < maxfd; fd++ )
            close( fd );
    }

    if ( !( flags & BD_NO_REOPEN_STD_FDS ) )
    {
        close( STDIN_FILENO );          /* Reopen standard fd's to /dev/null */

        fd = open( "/dev/null", O_RDWR );

        if ( fd != STDIN_FILENO )       /* 'fd' should be 0 */
            return -1;

        if ( dup2( STDIN_FILENO, STDOUT_FILENO ) != STDOUT_FILENO )
            return -1;

        if ( dup2( STDIN_FILENO, STDERR_FILENO ) != STDERR_FILENO )
            return -1;
    }

    return 0;
}

/** @brief get_all_interfaces
  *
  * @todo: document this function
  */
map<string, InterfaceInfo> LinuxUtils::get_all_interfaces( void )
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
                             nullptr, 0U, NI_NUMERICHOST );

            if ( s != 0 )
            {
                continue;
            }

            string interface_name( ifaddr->ifa_name );

            string mac=get_mac( ifaddr->ifa_name );

            if ( interfaces.find( mac ) == interfaces.end() )
            {
                InterfaceInfo in;
                in.set_name( ifaddr->ifa_name );
                in.set_mac( mac.c_str() );
                in.set_ip4( host );
                interfaces[mac] = in;
            }
            else
            {
                interfaces[mac].set_ip4( host );
            }
        }
        else if ( family == AF_INET6 )
        {
            s = getnameinfo( ifaddr->ifa_addr,
                             ( family == AF_INET ) ? sizeof( struct sockaddr_in ) :
                             sizeof( struct sockaddr_in6 ),
                             host, NI_MAXHOST,
                             nullptr, 0U, NI_NUMERICHOST );

            if ( s != 0 )
            {
                continue;
            }

            string interface_name = string( ifaddr->ifa_name );

            string mac=get_mac( ifaddr->ifa_name );

            if ( interfaces.find( mac ) == interfaces.end() )
            {
                InterfaceInfo in;
                in.set_name( ifaddr->ifa_name );
                in.set_mac( mac.c_str() );
                in.set_ip6( host );
                interfaces[interface_name] = in;
            }
            else
            {
                interfaces[mac].set_ip6( host );
            }
        }
    }

    delete []host;

    freeifaddrs( ipa );

    return interfaces;
}


/** @brief get_mac
  *
  * @todo: document this function
  */
string LinuxUtils::get_mac( char* name )
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

/** @brief check_one_instance
  *
  * @todo: document this function
  */
bool LinuxUtils::check_one_instance( void )
{
    try
    {
        Globals::shared_mem.reset(
            new boost::interprocess::shared_memory_object( boost::interprocess::create_only, "SmartTrafficMeterSharedMemory", boost::interprocess::read_write ) );
    }
    catch ( ... )
    {
        return false;
    }

    return true;
}

/** @brief resolve
  *
  * @todo: document this function
  */
string LinuxUtils::resolve( const unsigned long address )
{
    string out;

#ifndef __pi__

    if ( !abfd )
    {
        char ename[1024];
        int l = readlink( "/proc/self/exe", ename, sizeof( ename ) );

        if ( l == -1 )
        {
            Logger::LogError( "Failed to find executable." );
            return out;
        }

        ename[l] = 0;

        bfd_init();

        abfd = bfd_openr( ename, nullptr );

        if ( !abfd )
        {
            Logger::LogError( "bfd_openr failed" );
            return out;
        }

        /* oddly, this is required for it to work... */
        bfd_check_format( abfd, bfd_object );

        unsigned storage_needed = bfd_get_symtab_upper_bound( abfd );
        syms = ( asymbol ** ) malloc( storage_needed );
        bfd_canonicalize_symtab( abfd, syms );

        text = bfd_get_section_by_name( abfd, ".text" );
    }

    long offset = ( ( unsigned long )address ) - text->vma;

    if ( offset > 0 )
    {
        const char *file;
        const char *func;
        unsigned line;

        if ( bfd_find_nearest_line( abfd, text, syms, offset, &file, &func, &line ) && file )
        {
            string file_str( file );

            out.clear();
            out += "file: ";
            out += file_str.substr( file_str.rfind( "/" ) + 1, string::npos );
            out += "\t";
            out += "func: ";
            out += func;
            out += "\t";
            out += "line: ";
            out += std::to_string( line );
        }
    }

#endif // __pi__
    return out;
}

/** @brief set_signals_handler
  *
  * @todo: document this function
  */
void LinuxUtils::set_signals_handler( void )
{
    signal( SIGINT, LinuxUtils::signal_handler );
    signal( SIGSEGV, LinuxUtils::signal_handler );
    signal( SIGTERM, LinuxUtils::signal_handler );
    signal( SIGABRT, LinuxUtils::signal_handler );
    signal( SIGILL, LinuxUtils::signal_handler );
    signal( SIGFPE, LinuxUtils::signal_handler );
}


/** @brief get_get_program_path
  *
  * @todo: document this function
  */
string LinuxUtils::get_get_program_path( const char* argv0 )
{
    char buf[1024] = {0};
    ssize_t size = readlink( "/proc/self/exe", buf, sizeof( buf ) );

    if ( size == 0 || size == sizeof( buf ) )
    {
        if ( argv0 == nullptr || argv0[0] == 0 )
        {
            return "";
        }

        boost::system::error_code ec;
        boost::filesystem::path p(
            boost::filesystem::canonical(
                argv0, boost::filesystem::current_path(), ec ) );
        return p.make_preferred().string();
    }
    else
    {
        std::string path( buf, size );
        boost::system::error_code ec;
        boost::filesystem::path p(
            boost::filesystem::canonical(
                path, boost::filesystem::current_path(), ec ) );
        return p.make_preferred().string();
    }
}

/** @brief get_user_host
  *
  * @todo: document this function
  */
void LinuxUtils::get_user_host(string& user, string& host)
{
        char hostname[HOST_NAME_MAX];
        char username[LOGIN_NAME_MAX];

        memset(hostname,0,HOST_NAME_MAX*sizeof(char));
        memset(username,0,LOGIN_NAME_MAX*sizeof(char));

        gethostname( hostname, HOST_NAME_MAX );
        getlogin_r( username, LOGIN_NAME_MAX );

        user.clear();
        user.append(username);

        host.clear();
        host.append(hostname);
}


/** @brief get_os_info
  *
  * @todo: document this function
  */
void LinuxUtils::get_os_info(string& os_info)
{
	struct utsname info;
	memset(&info,0,sizeof(info));

	uname(&info);

	os_info.clear();
	os_info.append(info.sysname);
	os_info.append(", ");
	os_info.append(info.release);
	os_info.append(", ");
	os_info.append(info.version);
	os_info.append(", ");
	os_info.append(info.machine);
}


#endif // __linux
