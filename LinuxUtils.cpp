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
#include <stdlib.h>

#include "Utils.h"
#include "Logger.h"
#include "Debug.h"
#include "Globals.h"
#include "Settings.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"

#include "LinuxUtils.h"

using namespace std;

/** @brief MeterThread
  *
  * @todo: document this function
  */
void* LinuxUtils::MeterThread(void)
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
                                 NULL, 0, NI_NUMERICHOST );

                if ( s != 0 )
                {
                    continue;
                }

            }
            else if ( family == AF_PACKET && ifaddr->ifa_data != NULL )
            {
                struct rtnl_link_stats *stats = ( rtnl_link_stats * ) ifaddr->ifa_data;

//				printf ( "\t\ttx_packets = %10u; rx_packets = %10u\n"
//						 "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n",
//						 stats->tx_packets, stats->rx_packets,
//						 stats->tx_bytes, stats->rx_bytes );

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
        gettimeofday( &te, NULL );
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

    return 0;
}

/** @brief signal_handler
  *
  * @todo: document this function
  */
void LinuxUtils::signal_handler(int signal)
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

    exit( 0 );
}

