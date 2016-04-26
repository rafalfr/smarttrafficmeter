#include "config.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <map>
#include <csignal>
#include <pthread.h>
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
#include "version.h"
#include "Utils.h"
#include "Globals.h"
#include "Debug.h"
#include "Logger.h"
#include "Settings.h"
#include "BecomeDaemon.h"
#include "MySQLInterface.h"
#include "ServerThread.h"
#include "InterfaceInfo.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"
#include "WebSiteContent.h"
#include "server_http.hpp"
#include "tclap/CmdLine.h"

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

//https://community.smartthings.com/t/grovestreams-as-an-alternative-to-xively/6314


//http://stackoverflow.com/questions/10943907/linux-allocator-does-not-release-small-chunks-of-memory/10945602#10945602
//https://github.com/skanzariya/Memwatch

//https://regex101.com/

//https://github.com/eidheim/Simple-Web-Server

// for backtrace code to work compile with -g -rdynamic options

//required packages
//libsqlite3-dev
//libmysqlclient-dev
//cbp2make
//binutils-dev
//libboost1.58-dev (libboost1.50-dev raspberry pi)
//libboost-context-dev libboost-coroutine-dev libboost-regex-dev libboost-thread-dev libboost-system-dev

/* generowanie pliku makefile i kompilacja za pomocą make
cbp2make -in SmartTrafficMeter.cbp
make clean -f SmartTrafficMeter.cbp.mak
make -f SmartTrafficMeter.cbp.mak
*/

pthread_t t1;
pthread_t t2;

bool is_daemon = false;
string cwd;

//mac, table, date, stats
map<string, map<string, map<string, InterfaceStats> > > all_stats;
map<string, InterfaceSpeedMeter> speed_stats;
map<string, string> table_columns;

void save_stats_to_mysql( void );
void save_stats_to_sqlite( void );
void save_stats_to_files( void );
void load_data_from_sqlite( void );
void load_data_from_files( void );
void load_settings( void );
int32_t dirExists( const char *path );
int32_t mkpath( const std::string& s, mode_t mode );
static void signal_handler( int );
static void * MeterThread( void *arg );
void get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h );

int main( int argc, char *argv[] )
{
	uint32_t y;
	uint32_t m;
	uint32_t d;
	uint32_t h;
	void *res;
	int32_t s;

	all_stats.clear();
	speed_stats.clear();
	table_columns.clear();
	Settings::settings.clear();


	string version = string( AutoVersion::FULLVERSION_STRING ) + " built on " + string( __DATE__ ) + " " + string( __TIME__ );

	try
	{
		CmdLine cmd( "Command description message", ' ', version );

		SwitchArg daemon_switch( "d", "daemon", "run the program as daemon", false, NULL );
		cmd.add( daemon_switch );
		cmd.parse( argc, argv );

		is_daemon = daemon_switch.getValue();
	}
	catch ( ArgException &e ) // catch any exceptions
	{
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
	}

	cout << "Smart Traffic Meter version " << version << endl;

	char buf[512];

	if ( getcwd( buf, 512 * sizeof( char ) ) != nullptr )
	{
		cwd.clear();
		cwd.append( buf );
	}

	if ( is_daemon == true )
	{
		if ( BecomeDaemon( BD_NO_CHDIR | BD_NO_UMASK0 ) == -1 )
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

	//install handlers
	signal( SIGINT, signal_handler );
	signal( SIGSEGV, signal_handler );
	signal( SIGTERM, signal_handler );
	signal( SIGABRT, signal_handler );
	signal( SIGILL, signal_handler );
	signal( SIGFPE, signal_handler );

	//set default settings
	Settings::settings["storage"] = "sqlite";
	Settings::settings["database directory"] = cwd;
	Settings::settings["stats refresh interval"] = "1";	//seconds
	Settings::settings["stats save interval"] = "1800";	//seconds
	Settings::settings["stats server port"] = "32000";
	Settings::settings["html server port"] = "80";

	Globals::db_drv.set_database_type(Settings::settings["storage"]);
	Globals::db_drv.set_database_dir(Settings::settings["database directory"]);

	load_settings();

	const map<string, InterfaceInfo>& interfaces = Utils::get_all_interfaces();

	string row;

	for ( auto const & mac_info : interfaces )
	{
		const string& mac = mac_info.second.get_mac();

		InterfaceSpeedMeter ism;
		speed_stats[mac] = ism;

		if ( all_stats.find( mac ) == all_stats.end() )
		{
			get_time( &y, &m, &d, &h );

			InterfaceStats hstats;
			row.clear();
			row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + "_" + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";
			all_stats[mac]["hourly"][row] = hstats;

			InterfaceStats dstats;
			row.clear();
			row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );
			all_stats[mac]["daily"][row] = dstats;

			InterfaceStats mstats;
			row.clear();
			row += std::to_string( y ) + "-" + std::to_string( m );
			all_stats[mac]["monthly"][row] = mstats;

			InterfaceStats ystats;
			row.clear();
			row += std::to_string( y );
			all_stats[mac]["yearly"][row] = ystats;
		}
	}

	const string& storage = Settings::settings["storage"];

	if ( Utils::starts_with( storage, "mysql" ) )
	{
#ifdef use_mysql
		save_stats_to_mysql();
#endif // use_mysql
	}

	if ( Utils::starts_with( storage, "sqlite" ) )
	{
#ifdef use_sqlite
		load_data_from_sqlite();
#endif // use_sqlite
	}

	if ( Utils::starts_with( storage, "files" ) )
	{
		load_data_from_files();
	}

	int st = pthread_create( &t2, NULL, &ServerThread::Thread, NULL );

	if ( st != 0 )
	{
		cout << "Can't start server thread" << endl;
		Logger::LogError( "Can't start server thread" );
	}

//http://stackoverflow.com/questions/17642433/why-pthread-causes-a-memory-leak

	s = pthread_create( &t1, NULL, &MeterThread, NULL );

	if ( s != 0 )
	{
		Logger::LogError( "Can't start monitoring thread" );
		return 1;
	}

	cout << "Monitoring has started" << endl;

//	s = pthread_join( t2, &res );
//
//	if ( s != 0 )
//	{
//		Logger::LogError( "Can't join server thread" );
//		return 1;
//	}

	SimpleWeb::Server<SimpleWeb::HTTP> http_server( 8080, 2 );
	WebSiteContent::set_web_site_content( http_server );

	thread server_thread( [&http_server]()
	{
		http_server.start();
	} );

	s = pthread_join( t1, &res );

	if ( s != 0 )
	{
		Logger::LogError( "Can't join monitoring thread" );
		return 1;
	}

	exit( EXIT_SUCCESS );
}

static void * MeterThread( void * )
{

	uint32_t y;
	uint32_t m;
	uint32_t d;
	uint32_t h;
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

				get_time( &y, &m, &d, &h );

				//TODO remove possible memory leak
				const string& mac = Utils::get_mac( ifaddr->ifa_name ).c_str();

				if ( speed_stats.find( mac ) == speed_stats.end() )
				{
					///TODO load data from database
					InterfaceSpeedMeter ism;
					speed_stats[mac] = ism;
				}

				row.clear();
				row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + "_" + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";

				if ( all_stats[mac]["hourly"].find( row ) == all_stats[mac]["hourly"].end() )
				{
					InterfaceStats hstats;
					all_stats[mac]["hourly"][row] = hstats;
				}

				all_stats[mac]["hourly"][row].update( stats->tx_bytes, stats->rx_bytes );


				row.clear();
				row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );

				if ( all_stats[mac]["daily"].find( row ) == all_stats[mac]["daily"].end() )
				{
					InterfaceStats dstats;
					all_stats[mac]["daily"][row] = dstats;
				}

				all_stats[mac]["daily"][row].update( stats->tx_bytes, stats->rx_bytes );

				row.clear();
				row += std::to_string( y ) + "-" + std::to_string( m );

				if ( all_stats[mac]["monthly"].find( row ) == all_stats[mac]["monthly"].end() )
				{
					InterfaceStats mstats;
					all_stats[mac]["monthly"][row] = mstats;
				}

				all_stats[mac]["monthly"][row].update( stats->tx_bytes, stats->rx_bytes );


				row.clear();
				row += std::to_string( y );

				if ( all_stats[mac]["yearly"].find( row ) == all_stats[mac]["yearly"].end() )
				{
					InterfaceStats ystats;
					all_stats[mac]["yearly"][row] = ystats;
				}

				all_stats[mac]["yearly"][row].update( stats->tx_bytes, stats->rx_bytes );

				speed_stats[mac].update( stats->rx_bytes, stats->tx_bytes );

				if ( is_daemon == false )
				{
					for ( auto const & mac_speedinfo : speed_stats )
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

		if ( c_time >= p_time + ( 1000ULL * save_interval ) )
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
				save_stats_to_sqlite();
#endif // use_sqlite
			}

			if ( Utils::contians( storage, "files" ) )
			{
				save_stats_to_files();
			}

			/* remove unused rows from the all_stats container */

			for ( auto const & mac_table : all_stats )
			{
				const string& mac = mac_table.first;

				const map<string, map<string, InterfaceStats> > & table = mac_table.second;

				for ( auto const & table_row : table )
				{
					const string& table_name = table_row.first;	//hourly, daily, etc..

					current_row.clear();

					if ( table_name.compare( "hourly" ) == 0 )
					{
						current_row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + "_" + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";
					}
					else if ( table_name.compare( "daily" ) == 0 )
					{
						current_row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );
					}
					else if ( table_name.compare( "monthly" ) == 0 )
					{
						current_row += std::to_string( y ) + "-" + std::to_string( m );
					}
					else if ( table_name.compare( "yearly" ) == 0 )
					{
						current_row += std::to_string( y );
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
						all_stats[mac][table_name].erase( rows4remove[i] );
					}

					rows4remove.clear();
				}
			}

			p_time = c_time;
		}

		sleep( refresh_interval );
	}

	return 0;
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

	if ( s[s.size() - 1] != '/' )
	{
		s += '/';
	}

	while ( ( pos = s.find_first_of( '/', pre ) ) != string::npos )
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

void get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h )
{
	time_t t = time( NULL );
	struct tm* tm = localtime( &t );
	( *y ) = tm->tm_year + 1900;
	( *m ) = tm->tm_mon + 1;
	( *d ) = tm->tm_mday;
	( *h ) = tm->tm_hour;
}

void save_stats_to_files( void )
{
	for ( auto const & mac_table : all_stats )
	{
		const string& mac = mac_table.first;
		mkpath( mac, 0755 );

		const map<string, map<string, InterfaceStats> > & table = mac_table.second;

		for ( auto const & table_row : table )
		{
			const string& table_name = table_row.first;
			mkpath( mac + "/" + table_name, 0755 );

			const map<string, InterfaceStats> & row = table_row.second;

			for ( auto const & row_stats : row )
			{
				const string& row = row_stats.first;
				mkpath( mac + "/" + table_name + "/" + row, 0755 );

				const InterfaceStats& stats = row_stats.second;

				ofstream file;
				file.open( cwd + "/" + mac + "/" + table_name + "/" + row + "/stats.txt" );

				if ( file.is_open() == true )
				{
					uint64_t rx = stats.recieved();
					uint64_t tx = stats.transmited();

					file << ( Utils::to_string( rx ) );
					file << endl;
					file << ( Utils::to_string( tx ) );

					file.close();
				}
			}
		}
	}
}

#ifdef use_sqlite
static int callback( void *, int argc, char **argv, char **azColName )
{
	int i;

	if ( argc > 0 )
	{
		for ( i = 0; i < argc; i++ )
		{
			table_columns[string( azColName[i] )] = string( argv[i] ? argv[i] : "0" );
		}

		return 0;
	}
	else
	{
		return 1;
	}
}
#endif


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
//				printf( "can't create table" );
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
					printf( "error\n" );
					continue;
				}
			}
		}

		mysql_close( conn );
	}

#endif // use_mysql
}




void save_stats_to_sqlite( void )
{
#ifdef use_sqlite
//http://stackoverflow.com/questions/18794580/mysql-create-table-if-not-exists-in-phpmyadmin-import
//http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm
//https://www.sqlite.org/cintro.html

	string query;

	for ( auto const & mac_table : all_stats )
	{
		sqlite3 *db;
		char *zErrMsg = 0;
		int rc;

		const string& mac = mac_table.first;
		const map<string, map<string, InterfaceStats> > & table = mac_table.second;

		rc = sqlite3_open_v2( ( cwd + "/" + mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );

		if ( rc != SQLITE_OK )
		{
			continue;
		}

		for ( auto const & table_row : table )
		{
			const string& table_name = table_row.first;

			query.clear();
			query += "CREATE TABLE IF NOT EXISTS '" + table_name + "' ('row' VARCHAR(45) NULL,'rx_bytes' UNSIGNED BIG INT NULL,'tx_bytes' UNSIGNED BIG INT NULL,PRIMARY KEY ('row'));";

			rc = sqlite3_exec( db, query.c_str(), callback, 0, &zErrMsg );

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

void load_data_from_files( void )
{
	uint32_t y;
	uint32_t m;
	uint32_t d;
	uint32_t h;
	uint64_t rx_bytes;
	uint64_t tx_bytes;

	get_time( &y, &m, &d, &h );

	const map<string, InterfaceInfo>& interfaces = Utils::get_all_interfaces();

	string row;
	ifstream file;

	for ( auto const & kv : interfaces )
	{
		const InterfaceInfo& in = kv.second;
		const string& mac = in.get_mac();

///
		row.clear();
		row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + "_" + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";
		file.open( cwd + "/" + mac + "/hourly/" + row + "/stats.txt", std::ifstream::in );

		if ( file.is_open() == true )
		{
			file >> rx_bytes;
			file >> tx_bytes;
			file.close();
			all_stats[mac]["hourly"][row].set_initial_stats( tx_bytes, rx_bytes );
		}

///
		row.clear();
		row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );
		file.open( cwd + "/" + mac + "/daily/" + row + "/stats.txt", ifstream::in );

		if ( file.is_open() == true )
		{
			file >> rx_bytes;
			file >> tx_bytes;
			file.close();
			all_stats[mac]["daily"][row].set_initial_stats( tx_bytes, rx_bytes );
		}

///
		row.clear();
		row += std::to_string( y ) + "-" + std::to_string( m );
		file.open( cwd + "/" + mac + "/monthly/" + row + "/stats.txt", ifstream::in );

		if ( file.is_open() == true )
		{
			file >> rx_bytes;
			file >> tx_bytes;
			file.close();
			all_stats[mac]["monthly"][row].set_initial_stats( tx_bytes, rx_bytes );
		}

///
		row.clear();
		row += std::to_string( y );
		file.open( cwd + "/" + mac + "/yearly/" + row + "/stats.txt", ifstream::in );

		if ( file.is_open() == true )
		{
			file >> rx_bytes;
			file >> tx_bytes;
			file.close();
			all_stats[mac]["yearly"][row].set_initial_stats( tx_bytes, rx_bytes );
		}
	}
}



void load_data_from_sqlite( void )
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

	get_time( &y, &m, &d, &h );

	const map<string, InterfaceInfo>& interfaces = Utils::get_all_interfaces();

	string row;
	string query;

	for ( auto const & kv : interfaces )
	{
		const InterfaceInfo& in = kv.second;
		const string& mac = in.get_mac();

		rc = sqlite3_open_v2( ( cwd + "/" + mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE, NULL );

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
		query += std::to_string( y );
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
			all_stats[mac]["yearly"][row] = ystats;
			all_stats[mac]["yearly"][row].set_initial_stats( tx_bytes, rx_bytes );
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
		query += std::to_string( y ) + "-" + std::to_string( m );
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
			all_stats[mac]["monthly"][row] = mstats;
			all_stats[mac]["monthly"][row].set_initial_stats( tx_bytes, rx_bytes );
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
		query += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );
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
			all_stats[mac]["daily"][row] = dstats;
			all_stats[mac]["daily"][row].set_initial_stats( tx_bytes, rx_bytes );
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
		query += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + "_" + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";
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
			all_stats[mac]["hourly"][row] = hstats;
			all_stats[mac]["hourly"][row].set_initial_stats( tx_bytes, rx_bytes );
		}

		sqlite3_close( db );
	}

#endif // use_sqlite
}

/** @brief load_settings
  *
  * @todo: document this function
  */
void load_settings( void )
{
	//mkpath( "/etc/smarttrafficmeter", 0644 );

	ifstream file;
	file.open( Settings::settings["database directory"] + "/smartrafficmeter.conf", std::ifstream::in );

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

static void signal_handler( int signal )
{
	//pthread_kill(t1, SIGTERM);
	//pthread_kill(t2, SIGTERM);

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
		save_stats_to_sqlite();
#endif // use_sqlite
	}

	if ( Utils::contians( storage, "files" ) )
	{
		save_stats_to_files();
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
