/*

main.cpp

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
#include "GroveStreamsUploader.h"
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

void load_settings ( void );

int main ( int argc, char *argv[] )
{
	uint32_t y;
	uint32_t m;
	uint32_t d;
	uint32_t h;
	bool make_program_run_at_startup = false;

	Globals::all_stats.clear();
	Globals::speed_stats.clear();
	Settings::settings.clear();

	string version = string ( AutoVersion::FULLVERSION_STRING ) + " built on " + string ( __DATE__ ) + " " + string ( __TIME__ );

	try
	{
		CmdLine cmd ( "Command description message", ' ', version );

		SwitchArg daemon_switch ( "d", "daemon", "run the program as daemon", false, nullptr );
		cmd.add ( daemon_switch );

		SwitchArg startup_switch ( "s", "startup", "make the program run at computer startup (currently only Windows supported)", false, nullptr );
		cmd.add ( startup_switch );

		cmd.parse ( argc, argv );

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

	Globals::program_path = Utils::get_program_path ( argv );
	Globals::cwd = Utils::get_path ( Globals::program_path );

	if ( chdir ( Globals::cwd.c_str() ) == -1 )
	{
		Logger::LogError ( "cannot change working directory" );
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
			Logger::LogError ( "Can't start as daemon. Process exited" );
			return 0;
		}
		else
		{
			Logger::LogInfo ( "SmartTrafficMeter has started as a daemon" );
		}
	}

	if ( Utils::check_one_instance() == false )
	{
		Logger::LogError ( "Another instance is already running. Process has finished." );
		return 0;
	}

	Utils::save_pid_file ( Globals::cwd + PATH_SEPARATOR_CAHR + "stm.pid" );

	if ( Globals::is_daemon == true )
	{
		Utils::sleep_seconds ( 10 );
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
	Settings::settings["grovestreams api key"] = "";

	load_settings();

	Globals::db_drv.set_database_type ( Settings::settings["storage"] );
	Globals::db_drv.set_database_dir ( Settings::settings["database directory"] );

	Globals::interfaces = Utils::get_all_interfaces();

	string row;

	for ( auto const & mac_info : Globals::interfaces )
	{
		const string& mac = mac_info.second.get_mac();

		InterfaceSpeedMeter ism;
		Globals::speed_stats[mac] = ism;

		if ( Globals::all_stats.find ( mac ) == Globals::all_stats.end() )
		{
			Utils::get_time ( &y, &m, &d, &h );

			InterfaceStats hstats;
			row.clear();
			row += Utils::to_string ( y ) + "-" + Utils::to_string ( m, 2 ) + "-" + Utils::to_string ( d, 2 ) + "_" + Utils::to_string ( h, 2 ) + ":00-" + Utils::to_string ( h + 1, 2 ) + ":00";
			Globals::all_stats[mac]["hourly"][row] = hstats;

			InterfaceStats dstats;
			row.clear();
			row += Utils::to_string ( y ) + "-" + Utils::to_string ( m, 2 ) + "-" + Utils::to_string ( d, 2 );
			Globals::all_stats[mac]["daily"][row] = dstats;

			InterfaceStats mstats;
			row.clear();
			row += Utils::to_string ( y ) + "-" + Utils::to_string ( m, 2 );
			Globals::all_stats[mac]["monthly"][row] = mstats;

			InterfaceStats ystats;
			row.clear();
			row += Utils::to_string ( y );
			Globals::all_stats[mac]["yearly"][row] = ystats;
		}
	}

	const string& storage = Settings::settings["storage"];

	if ( Utils::starts_with ( storage, "mysql" ) )
	{
#ifdef use_mysql
#endif // use_mysql
	}

	if ( Utils::starts_with ( storage, "sqlite" ) )
	{
#ifdef use_sqlite
		Utils::load_data_from_sqlite();
#endif // use_sqlite
	}

	if ( Utils::starts_with ( storage, "files" ) )
	{
		Utils::load_data_from_files();
	}

	if ( Utils::contians ( storage, "mysql" ) )
	{
#ifdef use_mysql
		//save_stats_to_mysql();
#endif // use_mysql
	}

	if ( Utils::contians ( storage, "sqlite" ) )
	{
#ifdef use_sqlite
		Utils::save_stats_to_sqlite();
#endif // use_sqlite
	}

	if ( Utils::contians ( storage, "files" ) )
	{
		Utils::save_stats_to_files();
	}


	boost::thread stats_server_thread ( ServerThread::Thread );

	boost::thread meter_thread ( Utils::MeterThread );

	if ( Globals::is_daemon == false )
	{
		cout << "Monitoring has started" << endl;
	}

#ifndef _NO_WEBSERVER
	SimpleWeb::Server<SimpleWeb::HTTP> http_server ( Utils::stoi ( Settings::settings["html server port"] ), 2 );
	WebSiteContent::set_web_site_content ( http_server );

	boost::thread web_server_thread ( [&http_server]()
	{
		http_server.start();
	} );
#endif // _NO_WEBSERVER

	meter_thread.join();

	exit ( EXIT_SUCCESS );
}

/** @brief load_settings
  *
  * @todo: document this function
  */
void load_settings ( void )
{
	ifstream file;
	file.open ( Globals::cwd + PATH_SEPARATOR + "smarttrafficmeter.conf", std::ifstream::in );

	if ( file.is_open() )
	{
		for ( string line; getline ( file, line ); )
		{
			// skip over comment lines
			if ( Utils::starts_with ( line, "#" ) )
			{
				continue;
			}

			line=Utils::trim(line);

			// check if line matches settings pattern
			if ( regex_match ( line, regex ( "^[a-z\\s]+\\=[\\x2F\\x5Ca-z\\s0-9\\.\\-]+$" ) ) )
			{
				const vector<string>& items = Utils::split ( line, "=" );
				Settings::settings[items[0]] = items[1];
			}
		}

		file.close();
	}

}
