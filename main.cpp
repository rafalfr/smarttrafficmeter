#include "config.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <map>
#include <csignal>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include "version.h"
#include "Utils.h"
#include "Globals.h"
#include "Debug.h"
#include "Logger.h"
#include "Settings.h"
#include "MySQLInterface.h"
#include "ServerThread.h"
#include "InterfaceInfo.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"
#include "tclap/CmdLine.h"

#ifndef _NO_WEBSERVER
#include "WebSiteContent.h"
#include "server_http.hpp"
#endif // _NO_WEBSERVER

#ifdef __WIN32
#include <direct.h>
#endif // __WIN32

#ifdef use_mysql

#endif // use_mysql

#ifdef use_sqlite
#include "sqlite3.h"
#endif // use_sqlite

using namespace std;
using namespace TCLAP;

//http://www.man7.org/tlpi/code/online/all_files_alpha.html
//http://man7.org/linux/man-pages/man3/getifaddrs.3.html
//http://stackoverflow.com/questions/1519585/how-to-get-mac-address-for-an-interface-in-linux-using-a-c-program
//http://stackoverflow.com/questions/18100097/portable-way-to-check-if-directory-exists-windows-linux-c
//http://stackoverflow.com/questions/675039/how-can-i-create-directory-tree-in-c-linux
//http://stackoverflow.com/questions/5339200/how-to-create-a-single-instance-application-in-c-or-c
//http://stackoverflow.com/questions/77005/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes/77336#77336
//http://stackoverflow.com/questions/15129089/is-there-a-way-to-dump-stack-trace-with-line-number-from-a-linux-release-binary
//http://www.opensource.apple.com/source/gdb/gdb-1515/src/binutils/addr2line.c
//https://gist.github.com/jvranish/4441299
//http://stackoverflow.com/questions/10520762/what-happens-with-mapiterator-when-i-remove-entry-from-map?lq=1
//http://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
//http://stackoverflow.com/questions/1528298/get-path-of-executable

//https://community.smartthings.com/t/grovestreams-as-an-alternative-to-xively/6314

//Windows
//http://www.codeproject.com/Articles/499465/Simple-Windows-Service-in-Cplusplus
//http://www.devx.com/cplus/Article/9857
//http://www.codeproject.com/Articles/5331/Retrieve-the-number-of-bytes-sent-received-and-oth
//http://dandar3.blogspot.com/2009/03/network-monitor-v07.html
//https://msdn.microsoft.com/en-us/library/windows/desktop/aa365915(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/aa365917(v=vs.85).aspx



//http://stackoverflow.com/questions/10943907/linux-allocator-does-not-release-small-chunks-of-memory/10945602#10945602
//https://github.com/skanzariya/Memwatch

//https://regex101.com/

//https://github.com/eidheim/Simple-Web-Server

//http://www.boost.org/doc/libs/1_41_0/more/getting_started/windows.html#or-build-binaries-from-source
//building boost for windows
//bootstrap.bat
//bjam toolset=gcc

// for backtrace code to work compile with -g -rdynamic options

//required packages
//libsqlite3-dev
//libmysqlclient-dev
//cbp2make
//binutils-dev
//libboost1.58-dev (libboost1.50-dev raspberry libboost-thread1.50-dev pi)
//libboost-context-dev libboost-coroutine-dev libboost-regex-dev libboost-thread-dev libboost-system-dev libboost-filesystem-dev

/* generowanie pliku makefile i kompilacja za pomocą make
cbp2make -in SmartTrafficMeter.cbp
make clean -f SmartTrafficMeter.cbp.mak
make -f SmartTrafficMeter.cbp.mak
*/

void save_stats_to_mysql( void );
void load_settings( void );
int32_t dirExists( const char *path );
int32_t mkpath( const std::string& s, mode_t mode );


int main( int argc, char *argv[] )
{
    uint32_t y;
    uint32_t m;
    uint32_t d;
    uint32_t h;
    bool make_program_run_at_startup = false;

    Globals::all_stats.clear();
    Globals::speed_stats.clear();
    Settings::settings.clear();

    string version = string( AutoVersion::FULLVERSION_STRING ) + " built on " + string( __DATE__ ) + " " + string( __TIME__ );

    try
    {
        CmdLine cmd( "Command description message", ' ', version );

        SwitchArg daemon_switch( "d", "daemon", "run the program as daemon", false, nullptr );
        cmd.add( daemon_switch );

        SwitchArg startup_switch( "s", "startup", "make the program run at computer startup (currently only Windows supported)", false, nullptr );
        cmd.add( startup_switch );

        cmd.parse( argc, argv );

        Globals::is_daemon = daemon_switch.getValue();
        make_program_run_at_startup = startup_switch.getValue();
    }
    catch ( ArgException &e ) // catch any exceptions
    {
        cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
    }

    if ( Globals::is_daemon == false )
    {
        cout << "Smart Traffic Meter version " << version << endl;
    }

    Globals::program_path = Utils::get_program_path( argv );
    Globals::cwd = Utils::get_path( Globals::program_path );

    if ( chdir( Globals::cwd.c_str() ) == -1 )
    {
        Logger::LogError( "cannot change working directory" );
    }

    if ( make_program_run_at_startup == true )
    {
        Utils::make_program_run_at_startup();
    }


    if ( Globals::is_daemon == true )
    {
        if ( Utils::BecomeDaemon() == -1 )
        {
            cout << "Can't start as daemon. Process exited" << endl;
            Logger::LogError( "Can't start as daemon. Process exited" );
            return 0;
        }
        else
        {
            Logger::LogInfo( "SmartTrafficMeter has started as daemon" );
        }
    }

    if ( Utils::check_one_instance() == false )
    {
        Logger::LogError( "Another instance is already running. Process exited" );
        return 0;
    }

    if ( Globals::is_daemon == true )
    {
        Utils::sleep_seconds( 10 );
    }

    Utils::set_signals_handler();
    Utils::set_endsession_handler();


    //set default settings
    Settings::settings["storage"] = "sqlite";
    Settings::settings["database directory"] = Globals::cwd;
    Settings::settings["stats refresh interval"] = "1";	//seconds
    Settings::settings["stats save interval"] = "1800";	//seconds
    Settings::settings["stats server port"] = "32000";
    Settings::settings["html server port"] = "8080";

    Globals::db_drv.set_database_type( Settings::settings["storage"] );
    Globals::db_drv.set_database_dir( Settings::settings["database directory"] );

    load_settings();

    Globals::interfaces = Utils::get_all_interfaces();

    string row;

    for ( auto const & mac_info : Globals::interfaces )
    {
        const string& mac = mac_info.second.get_mac();

        InterfaceSpeedMeter ism;
        Globals::speed_stats[mac] = ism;

        if ( Globals::all_stats.find( mac ) == Globals::all_stats.end() )
        {
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
        }
    }

    const string& storage = Settings::settings["storage"];

    if ( Utils::starts_with( storage, "mysql" ) )
    {
#ifdef use_mysql
        save_stats_to_mysql();
#endif // use_mysql
    }

    //save all stats
    if ( Utils::starts_with( storage, "sqlite" ) )
    {
#ifdef use_sqlite
        Utils::load_data_from_sqlite();
#endif // use_sqlite
    }

    if ( Utils::starts_with( storage, "files" ) )
    {
        Utils::load_data_from_files();
    }

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


    boost::thread stats_server_thread( ServerThread::Thread );

//http://stackoverflow.com/questions/17642433/why-pthread-causes-a-memory-leak


    boost::thread meter_thread( Utils::MeterThread );

    cout << "Monitoring has started" << endl;

#ifndef _NO_WEBSERVER
    SimpleWeb::Server<SimpleWeb::HTTP> http_server( Utils::stoi( Settings::settings["html server port"] ), 2 );
    WebSiteContent::set_web_site_content( http_server );

    boost::thread server_thread( [&http_server]()
    {
        http_server.start();
    } );
#endif // _NO_WEBSERVER

    meter_thread.join();

#ifdef __linux
    Globals::shared_mem.get()->remove( "SmartTrafficMeterSharedMemory" );
#endif // __linux

    exit( EXIT_SUCCESS );
}


int32_t dirExists( const char *path )
{
    struct stat info;

    if ( stat( path, &info ) != 0 )
    {
        return 0;
    }
    else if ( info.st_mode & S_IFDIR )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int32_t mkpath( const string& _s, mode_t mode )
{
    size_t pre = 0, pos;
    string dir;
    int32_t mdret = 0;
    string s( _s );

    if ( s[s.size() - 1] != PATH_SEPARATOR_CAHR )
    {
        s += '/';
    }

    while ( ( pos = s.find_first_of( PATH_SEPARATOR_CAHR, pre ) ) != string::npos )
    {
        dir = s.substr( 0, pos++ );
        pre = pos;

        if ( dir.size() == 0 )
        {
            continue;    // if leading / first time is 0 length
        }

#ifdef _WIN32

        if ( ( mdret = _mkdir( dir.c_str() ) ) && errno != EEXIST )
        {
            return mdret;
        }

#endif // _WIN32

#ifdef __linux

        if ( ( mdret = mkdir( dir.c_str(), mode ) ) && errno != EEXIST )
        {
            return mdret;
        }

#endif // __linux

    }

    return mdret;
}



void save_stats_to_mysql( void )
{
#ifdef use_mysql
    //http://zetcode.com/db/mysqlc/

    MYSQL *conn = mysql_init( NULL );


    for ( const auto & mac_table : all_stats )
    {
        bool res;


        string mac = mac_table.first;
        const map<string, map<string, InterfaceStats> > & table = mac_table.second;

        if ( mysql_real_connect( conn, "127.0.0.1", "root", "dvj5rfx2", NULL, 0, NULL, 0 ) != NULL )
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

        if ( mysql_real_connect( conn, "127.0.0.1", "root", "dvj5rfx2", mac.c_str(), 0, NULL, 0 ) != NULL )
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


/** @brief load_settings
  *
  * @todo: document this function
  */
void load_settings( void )
{
    ifstream file;
    file.open( Settings::settings["database directory"] + PATH_SEPARATOR + "smartrafficmeter.conf", std::ifstream::in );

    if ( file.is_open() )
    {
        for ( string line; getline( file, line ); )
        {
            // skip over comment lines
            if ( Utils::starts_with( line, "#" ) )
            {
                continue;
            }

            // check if line matches settings pattern
            if ( regex_match( line, regex( "^[a-z\\s]+\\=[a-z\\s0-9\\/\\.]+$" ) ) )
            {
                const vector<string>& items = Utils::split( line, "=" );
                Settings::settings[items[0]] = items[1];
            }
        }

        file.close();
    }

}
