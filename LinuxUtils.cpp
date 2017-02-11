/*

LinuxUtils.cpp

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
#include "GroveStreamsUploader.h"

#include "LinuxUtils.h"

#ifndef __pi__

bfd* LinuxUtils::abfd = nullptr;
asymbol ** LinuxUtils::syms = nullptr;
asection* LinuxUtils::text = nullptr;

#endif // __pi__


using namespace std;

/** @brief MeterThread
 *
 * The method which is run as a statistics monitoring thread
 *
 * @param void
 * @return void
 *
 */
void* LinuxUtils::MeterThread( void )
{
    bool first_iteration = true;
    uint32_t y = 0;
    uint32_t m = 0;
    uint32_t d = 0;
    uint32_t h = 0;
    uint32_t ph = 0;
    uint32_t num_running_interfaces = 0;
    uint32_t pnum_running_interfaces = 0;
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
    string current_hourly_row;
    string current_daily_row;
    string current_monthly_row;
    string current_yearly_row;
    string p_hourly_row;
    string p_daily_row;
    string p_monthly_row;
    string p_yearly_row;
    bool hourly_row_changed = false;
    bool daily_row_changed = false;
    bool monthly_row_changed = false;
    bool yearly_row_changed = false;


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
        save_interval = 30 * 60;	// save interval time in seconds
    }

    string row;
    string current_row;
    vector<map<string, InterfaceStats>::const_iterator> rows4remove;

    while ( Globals::terminate_program == false )
    {
        Utils::get_time( &y, &m, &d, &h );

        if ( first_iteration == true )
        {
            p_hourly_row = Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";
            p_daily_row = Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );
            p_monthly_row = Utils::to_string( y ) + "-" + Utils::to_string( m, 2 );
            p_yearly_row = Utils::to_string( y );
            first_iteration = false;
        }

        hourly_row_changed = daily_row_changed = monthly_row_changed = yearly_row_changed = false;

        if ( getifaddrs( &ifaddr ) == -1 )
        {
            freeifaddrs( ifaddr );
            continue;
        }

        ipa = ifaddr;

        for ( num_running_interfaces = 0 ; ifaddr != NULL; ifaddr = ifaddr->ifa_next )
        {

            if ( ifaddr->ifa_addr == NULL || strcmp( ifaddr->ifa_name, "lo" ) == 0 || strcmp( ifaddr->ifa_name, "" ) == 0 || strlen( ifaddr->ifa_name ) == 0 )
            {
                continue;
            }

            bool interface_running = ( ifaddr->ifa_flags & IFF_RUNNING ) && ( ifaddr->ifa_flags & IFF_UP );

            if ( interface_running == true )
            {
                num_running_interfaces++;
            }
        }

        if ( num_running_interfaces > pnum_running_interfaces )
        {
            Globals::interfaces.clear();
            Globals::interfaces = Utils::get_all_interfaces();

            for ( auto & mac_interface : Globals::interfaces )
            {
                const string& mac = mac_interface.first;

                if ( Globals::all_stats.find( mac ) == Globals::all_stats.end() )
                {
                    string row;
                    Utils::get_time( &y, &m, &d, &h );

                    InterfaceStats hstats;
                    row.clear();
                    row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";
                    Globals::all_stats[mac]["hourly"][row] = hstats;

                    InterfaceStats dstats;
                    row.clear();
                    row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );
                    Globals::all_stats[mac]["daily"][row] = dstats;

                    InterfaceStats mstats;
                    row.clear();
                    row += Utils::to_string( y ) + "-" + Utils::to_string( m, 2 );
                    Globals::all_stats[mac]["monthly"][row] = mstats;

                    InterfaceStats ystats;
                    row.clear();
                    row += Utils::to_string( y );
                    Globals::all_stats[mac]["yearly"][row] = ystats;

                    Utils::load_stats( mac );
                }

                if ( Globals::session_stats.find( mac ) == Globals::session_stats.end() )
                {
                    InterfaceStats stats;
                    Globals::session_stats[mac] = stats;
                }
            }
        }

        if ( num_running_interfaces < pnum_running_interfaces )
        {
            Globals::interfaces.clear();
            Globals::interfaces = Utils::get_all_interfaces();

            for ( auto & mac_table : Globals::all_stats )
            {
                const string& mac = mac_table.first;

                if ( Globals::interfaces.find( mac ) == Globals::interfaces.end() )
                {
                    Utils::save_stats( mac );
                    map<string, map<string, InterfaceStats> > & table = mac_table.second;

                    for ( auto & table_row : table )
                    {
                        map<string, InterfaceStats> & row = table_row.second;

                        for ( auto & row_stats : row )
                        {
                            InterfaceStats& stats = row_stats.second;
                            stats.set_first_update( true );
                        }
                    }
                }
            }
        }

        ifaddr = ipa;

        for ( ; ifaddr != NULL; ifaddr = ifaddr->ifa_next )
        {

            if ( ifaddr->ifa_addr == NULL || strcmp( ifaddr->ifa_name, "lo" ) == 0 || strcmp( ifaddr->ifa_name, "" ) == 0 || strlen( ifaddr->ifa_name ) == 0 )
            {
                continue;
            }

            bool interface_running = ( ifaddr->ifa_flags & IFF_RUNNING ) && ( ifaddr->ifa_flags & IFF_UP );

            if ( interface_running == false )
            {
                continue;
            }

            family = ifaddr->ifa_addr->sa_family;

            if ( family == AF_PACKET && ifaddr->ifa_data != NULL )
            {
                const string& mac = Utils::get_mac( ifaddr->ifa_name ).c_str();

                if ( num_running_interfaces > pnum_running_interfaces )
                {
                    if ( Globals::speed_stats.find( mac ) == Globals::speed_stats.end() )
                    {
                        InterfaceSpeedMeter ism;
                        Globals::speed_stats[mac] = ism;
                    }
                }

                struct rtnl_link_stats *stats = ( rtnl_link_stats * ) ifaddr->ifa_data;

                Globals::session_stats[mac].update( stats->tx_bytes, stats->rx_bytes );

                current_hourly_row = Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 ) + "_" + Utils::to_string( h, 2 ) + ":00-" + Utils::to_string( h + 1, 2 ) + ":00";

                if ( Globals::all_stats[mac]["hourly"].find( current_hourly_row ) == Globals::all_stats[mac]["hourly"].end() )
                {
                    InterfaceStats hstats;
                    Globals::all_stats[mac]["hourly"][current_hourly_row] = hstats;

                    Globals::all_stats[mac]["hourly"][p_hourly_row].update( stats->tx_bytes, stats->rx_bytes );
                    hourly_row_changed = true;
                }

                Globals::all_stats[mac]["hourly"][current_hourly_row].update( stats->tx_bytes, stats->rx_bytes );


                current_daily_row = Utils::to_string( y ) + "-" + Utils::to_string( m, 2 ) + "-" + Utils::to_string( d, 2 );

                if ( Globals::all_stats[mac]["daily"].find( current_daily_row ) == Globals::all_stats[mac]["daily"].end() )
                {
                    InterfaceStats dstats;
                    Globals::all_stats[mac]["daily"][current_daily_row] = dstats;

                    Globals::all_stats[mac]["daily"][p_daily_row].update( stats->tx_bytes, stats->rx_bytes );
                    daily_row_changed = true;
                }

                Globals::all_stats[mac]["daily"][current_daily_row].update( stats->tx_bytes, stats->rx_bytes );


                current_monthly_row = Utils::to_string( y ) + "-" + Utils::to_string( m, 2 );

                if ( Globals::all_stats[mac]["monthly"].find( current_monthly_row ) == Globals::all_stats[mac]["monthly"].end() )
                {
                    InterfaceStats mstats;
                    Globals::all_stats[mac]["monthly"][current_monthly_row] = mstats;

                    Globals::all_stats[mac]["monthly"][p_monthly_row].update( stats->tx_bytes, stats->rx_bytes );
                    monthly_row_changed = true;
                }

                Globals::all_stats[mac]["monthly"][current_monthly_row].update( stats->tx_bytes, stats->rx_bytes );


                current_yearly_row = Utils::to_string( y );

                if ( Globals::all_stats[mac]["yearly"].find( current_yearly_row ) == Globals::all_stats[mac]["yearly"].end() )
                {
                    InterfaceStats ystats;
                    Globals::all_stats[mac]["yearly"][current_yearly_row] = ystats;

                    Globals::all_stats[mac]["yearly"][p_yearly_row].update( stats->tx_bytes, stats->rx_bytes );
                    yearly_row_changed = true;
                }

                Globals::all_stats[mac]["yearly"][current_yearly_row].update( stats->tx_bytes, stats->rx_bytes );

                Globals::speed_stats[mac].update( stats->rx_bytes, stats->tx_bytes );

                if ( Globals::is_daemon == false )
                {
                    for ( auto const & mac_speedinfo : Globals::speed_stats )
                    {
                        const string& interface_mac = mac_speedinfo.first;
                        const InterfaceSpeedMeter& ism = mac_speedinfo.second;

                        cout << interface_mac << endl;

                        double speed = ( double ) ism.get_tx_speed();

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

                        speed = ( double ) ism.get_rx_speed();

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

//            if ( Globals::upload_threads.size() > 0 )
//            {
//                for ( const auto & th : Globals::upload_threads )
//                {
//                    delete th;
//                }
//
//                Globals::upload_threads.clear();
//            }
//
//            Globals::upload_threads.push_back( new boost::thread( GroveStreamsUploader::run ) );

            Utils::save_stats( "" );

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
                        current_row += current_hourly_row;
                    }
                    else if ( table_name.compare( "daily" ) == 0 )
                    {
                        current_row += current_daily_row;
                    }
                    else if ( table_name.compare( "monthly" ) == 0 )
                    {
                        current_row += current_monthly_row;
                    }
                    else if ( table_name.compare( "yearly" ) == 0 )
                    {
                        current_row += current_yearly_row;
                    }

                    const map<string, InterfaceStats> & crow = table_row.second;

                    rows4remove.clear();

                    for ( map<string, InterfaceStats>::const_iterator it = crow.cbegin(); it != crow.cend(); )
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

        if ( hourly_row_changed == true )
        {
            p_hourly_row = current_hourly_row;
        }

        if ( daily_row_changed == true )
        {
            p_daily_row = current_daily_row;
        }

        if ( monthly_row_changed == true )
        {
            p_monthly_row = current_monthly_row;
        }

        if ( yearly_row_changed == true )
        {
            p_yearly_row = current_yearly_row;
        }

        pnum_running_interfaces = num_running_interfaces;

        sleep( refresh_interval );
    }

    return nullptr;
}

/** @brief signal_handler
 *
 * The method is invoked when a system signal is received
 *
 * @param signal
 * @return void
 *
 */
void LinuxUtils::signal_handler( int signal )
{
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

    Utils::save_stats( "" );

    Globals::terminate_program = true;
}

/** @brief BecomeDaemon
 *
 * The method makes the program to run as a linux daemon
 *
 * @param flags
 * @return daemon creation status
 *
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

    if ( !( flags & BD_NO_CLOSE_FILES ) )  /* Close all open files */
    {
        maxfd = sysconf( _SC_OPEN_MAX );

        if ( maxfd == -1 )              /* Limit is indeterminate... */
            maxfd = BD_MAX_CLOSE;       /* so take a guess */

        for ( fd = 0; fd < maxfd; fd++ )
            close( fd );
    }

//	if ( ! ( flags & BD_NO_REOPEN_STD_FDS ) )
//	{
//		close ( STDIN_FILENO );         /* Reopen standard fd's to /dev/null */
//
//		fd = open ( "/dev/null", O_RDWR );
//
//		if ( fd != STDIN_FILENO )       /* 'fd' should be 0 */
//			return -1;
//
//		if ( dup2 ( STDIN_FILENO, STDOUT_FILENO ) != STDOUT_FILENO )
//			return -1;
//
//		if ( dup2 ( STDIN_FILENO, STDERR_FILENO ) != STDERR_FILENO )
//			return -1;
//	}

    return 0;
}

/** @brief get_all_interfaces
 *
 * The method returns all available network interfaces
 *
 * @param void
 * @return all available network interfaces
 *
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
        if ( ifaddr->ifa_addr == NULL || strcmp( ifaddr->ifa_name, "lo" ) == 0 || strcmp( ifaddr->ifa_name, "" ) == 0 || strlen( ifaddr->ifa_name ) == 0 )
        {
            continue;
        }

        if ( !( ifaddr->ifa_flags & IFF_RUNNING ) )
        {
            continue;
        }

        family = ifaddr->ifa_addr->sa_family;


        /* For an AF_INET* interface address, display the address */

        if ( family == AF_INET )
        {
            for ( uint32_t i = 0; host[i] != '\0' && i < NI_MAXHOST; i++ )
            {
                host[i] = '\0';
            }

            s = getnameinfo( ifaddr->ifa_addr,
                             ( family == AF_INET ) ? sizeof( struct sockaddr_in ) :
                             sizeof( struct sockaddr_in6 ),
                             host, NI_MAXHOST,
                             nullptr, 0U, NI_NUMERICHOST );

            if ( s != 0 )
            {
                continue;
            }

            if ( strlen( host ) == 0 )
            {
                continue;
            }

            string interface_name( ifaddr->ifa_name );

            string mac = get_mac( ifaddr->ifa_name );

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
            memset( host, '\0', NI_MAXHOST * sizeof( char ) );

            s = getnameinfo( ifaddr->ifa_addr,
                             ( family == AF_INET ) ? sizeof( struct sockaddr_in ) :
                             sizeof( struct sockaddr_in6 ),
                             host, NI_MAXHOST,
                             nullptr, 0U, NI_NUMERICHOST );

            if ( s != 0 )
            {
                continue;
            }

            if ( strlen( host ) == 0 )
            {
                continue;
            }

            string interface_name = string( ifaddr->ifa_name );

            string mac = get_mac( ifaddr->ifa_name );

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
 * The method returns the MAC address of a network interface
 *
 * @param network interface name
 * @return MAC address
 *
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
 * The method checks whether another instance of the program is currently running
 *
 * @param void
 * @return true if this instance is the only one currently running, otherwise false is returned
 *
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

/** @brief remove_instance_object
 *
 * The method removes from the os a shared object which represents this program instance
 *
 * @param void
 * @return void
 *
 */
void LinuxUtils::remove_instance_object( void )
{
    Globals::shared_mem.get()->remove( "SmartTrafficMeterSharedMemory" );
}

/** @brief resolve
 *
 * The method resolves the execution address into program file, line and function
 *
 * @param execution address
 * @return program file, line and function
 *
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

    long offset = ( ( unsigned long ) address ) - text->vma;

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
 * The method sets the handler function for system signals
 *
 * @param void
 * @return void
 *
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

/** @brief get_program_path
 *
 * The method returns the program path
 *
 * @param argv[0]
 * @return full program path
 *
 */
string LinuxUtils::get_program_path( const char* argv0 )
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
 * The method returns the user name and host name
 *
 * @param[out] user name
 * @param[out] host name
 * @return void
 *
 */
void LinuxUtils::get_user_host( string& user, string& host )
{
    char hostname[HOST_NAME_MAX];
    char username[LOGIN_NAME_MAX];

    memset( hostname, 0, HOST_NAME_MAX * sizeof( char ) );
    memset( username, 0, LOGIN_NAME_MAX * sizeof( char ) );

    gethostname( hostname, HOST_NAME_MAX );
    getlogin_r( username, LOGIN_NAME_MAX );

    user.clear();

    if ( getuid() != 0 )
    {
        user.append( username );
    }
    else
    {
        user.append( "root" );
    }

    host.clear();
    host.append( hostname );
}

/** @brief get_os_info
 *
 * The method returns the operating system name
 *
 * @param[out] operating system name
 * @return void
 *
 */
void LinuxUtils::get_os_info( string& os_info )
{
    struct utsname info;
    memset( &info, 0, sizeof( info ) );

    uname( &info );

    os_info.clear();
    os_info.append( info.sysname );
    os_info.append( ", " );
    os_info.append( info.release );
    os_info.append( ", " );
    os_info.append( info.version );
    os_info.append( ", " );
    os_info.append( info.machine );
}

/** @brief dir_exists
 *
 * The method checks whether the given directory exists
 *
 * @param full directory path
 * @return true if the directory exists, false otherwise
 *
 */
bool LinuxUtils::dir_exists( const char* path )
{
    struct stat info;

    if ( stat( path, &info ) != 0 )
    {
        return false;
    }
    else if ( info.st_mode & S_IFDIR )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/** @brief make_path
 *
 * The method creates the given path
 *
 * @param full directory path to create
 * @param creation flags
 * @return creation status
 *
 */
int32_t LinuxUtils::make_path( const string& _s, mode_t mode )
{
    size_t pre = 0, pos;
    string dir;
    int32_t mdret = 0;
    string s( _s );

    if ( s[s.size() - 1] != PATH_SEPARATOR_CAHR )
    {
        s += PATH_SEPARATOR_CAHR;
    }

    while ( ( pos = s.find_first_of( PATH_SEPARATOR_CAHR, pre ) ) != string::npos )
    {
        dir = s.substr( 0, pos++ );
        pre = pos;

        if ( dir.size() == 0 )
        {
            continue;    // if leading / first time is 0 length
        }

        if ( ( mdret = mkdir( dir.c_str(), mode ) ) && errno != EEXIST )
        {
            return mdret;
        }
    }

    return mdret;
}

/** @brief save_pid_file
 *
 * The method saves the program pid to the pid file
 *
 * @param full pid file path
 * @return void
 *
 */
void LinuxUtils::save_pid_file( const string& pid_file_path )
{
    ofstream file;
    file.open( pid_file_path );

    if ( file.is_open() == true )
    {
        file << Utils::to_string( static_cast<uint64_t>( getpid() ), 1 );
        file.close();
    }
}


#endif // __linux
