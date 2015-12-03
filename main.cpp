#include "config.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <csignal>
#include <pthread.h>
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
#include <sys/signal.h>
#include <stdlib.h>
#include "version.h"
#include "Utils.h"
#include "BecomeDaemon.h"
#include "MySQLInterface.h"
#include "ServerThread.h"
#include "InterfaceInfo.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"

#ifdef use_mysql

#endif // use_mysql

#ifdef use_sqlite
#include "sqlite3.h"
#endif // use_sqlite

using namespace std;

//http://www.man7.org/tlpi/code/online/all_files_alpha.html
//http://man7.org/linux/man-pages/man3/getifaddrs.3.html
//http://stackoverflow.com/questions/1519585/how-to-get-mac-address-for-an-interface-in-linux-using-a-c-program
//http://stackoverflow.com/questions/18100097/portable-way-to-check-if-directory-exists-windows-linux-c
//http://stackoverflow.com/questions/675039/how-can-i-create-directory-tree-in-c-linux

//libsqlite3-dev
//libmysqlclient-dev
//cbp2make

/* generowanie pliku makefile i kompilacja za pomocą make
cbp2make -in SmartTrafficMeter.cbp
make clean -f SmartTrafficMeter.cbp.mak
make -f SmartTrafficMeter.cbp.mak
*/

pthread_t t1;
pthread_t t2;

bool is_daemon = false;
uint32_t refresh_interval = 1;  //statistics refresh interval in seconds
uint32_t save_interval = 30 * 60; //save interval in seconds
string cwd;

map<string, map<string, map<string, InterfaceStats> > > all_stats;
map<string, InterfaceSpeedMeter> speed_stats;
map<string, string> table_columns;
map<string, string> settings;



void save_stats_to_mysql( void );
void save_stats_to_sqlite( void );
void save_stats_to_files( void );
void load_data_from_sqlite( void );
void load_data_from_files( void );
int32_t dirExists( const char *path );
int32_t mkpath( const std::string s, mode_t mode );
static void signal_handler( int );
static void * MeterThread( void *arg );
void get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h );

int main( int argc, char *argv[] )
{
	if ( argc > 1 )
	{
		for ( int32_t i = 1; i < argc; i++ )
		{
			string arg = string( argv[i] );

			if ( arg.compare( "-daemon" ) == 0 )
			{
				is_daemon = true;
			}
		}
	}

	cout << "Smart Traffic Meter version " << AutoVersion::FULLVERSION_STRING << endl;

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
			return 0;
		}
	}

	void *res;
	int s;

	signal( SIGINT, signal_handler );
	signal( SIGSEGV, signal_handler );
	signal( SIGTERM, signal_handler );

	settings["storage"] = "sqlite files";

//    for(auto const & mac_table : all_stats)
//    {
//        string mac=mac_table.first;
//        cout<<mac<<endl;
//
//        for(auto const & table_rows : mac_table.second)
//        {
//            string table=table_rows.first;
//            cout<<table<<endl;
//
//            for(auto const & row_stats : table_rows.second)
//            {
//                string row=row_stats.first;
//                const InterfaceStats& stats=row_stats.second;
//                cout<<row<<"\t"<<stats.recieved()<<"\t"<<stats.transmited()<<endl;
//            }
//        }
//    }


	uint32_t y;
	uint32_t m;
	uint32_t d;
	uint32_t h;

	map<string, InterfaceInfo> interfaces = Utils::get_all_interfaces();

	for ( auto const & mac_info : interfaces )
	{
		string mac = mac_info.second.get_mac();

		InterfaceSpeedMeter ism;
		speed_stats[mac] = ism;

		if ( all_stats.find( mac ) == all_stats.end() )
		{
			get_time( &y, &m, &d, &h );

			InterfaceStats hstats;
			string row = std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + " " + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";
			all_stats[mac]["hourly"][row] = hstats;

			InterfaceStats dstats;
			row.clear();
			row = std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );
			all_stats[mac]["daily"][row] = dstats;

			InterfaceStats mstats;
			row.clear();
			row = std::to_string( y ) + "-" + std::to_string( m );
			all_stats[mac]["monthly"][row] = mstats;

			InterfaceStats ystats;
			row.clear();
			row = std::to_string( y );
			all_stats[mac]["yearly"][row] = ystats;
		}
	}

	string storage = settings["storage"];

	if ( Utils::starts_with(storage,"mysql") )
	{
#ifdef use_mysql
		save_stats_to_mysql();
#endif // use_mysql
	}
	else if ( Utils::starts_with(storage,"sqlite") )
	{
#ifdef use_sqlite
		load_data_from_sqlite();
#endif // use_sqlite
	}
	else if ( Utils::starts_with(storage,"files") )
	{
		load_data_from_files();
	}

//	for ( auto const & kv : interfaces )
//	{
//		std::cout << kv.second.get_name() << "\t" << kv.second.get_mac() << "\t" << kv.second.get_ip4() << "\t" << kv.second.get_ip6() << endl;
//		string parent_dir = kv.second.get_mac();
//		mkpath ( parent_dir, 0755 );
//		mkpath ( parent_dir + "/daily", 0755 );
//		mkpath ( parent_dir + "/monthly", 0755 );
//		mkpath ( parent_dir + "/yearly", 0755 );
//	}


	int st = pthread_create( &t2, NULL, &ServerThread::Thread, NULL );

	if (st!=0)
	{
		cout<<"Can't start server thread"<<endl;
	}

	//st = pthread_join( t2, &res );

//	if ( st != 0 )
//	{
//		return 1;
//	}

//http://stackoverflow.com/questions/17642433/why-pthread-causes-a-memory-leak

	s = pthread_create( &t1, NULL, &MeterThread, NULL );

	if ( s != 0 )
	{
		return 1;
	}

	cout << "Monitoring has started" << endl;

	s = pthread_join( t1, &res );

	if ( s != 0 )
	{
		return 1;
	}



	exit( EXIT_SUCCESS );
}

static void * MeterThread( void * )
{

	static uint64_t p_time = 0;
	struct ifaddrs *ifaddr, *ipa = nullptr;
	int family, s;
	char host[NI_MAXHOST];
	string hourly( "hourly" );
	string daily( "daily" );
	string monthly( "monthly" );
	string yearly( "yearly" );


	int k = 0;

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

				uint32_t y;
				uint32_t m;
				uint32_t d;
				uint32_t h;
				get_time( &y, &m, &d, &h );

				string mac = Utils::get_mac( ifaddr->ifa_name ).c_str();

				if ( speed_stats.find( mac ) == speed_stats.end() )
				{
					///TODO load data from database
					InterfaceSpeedMeter ism;
					speed_stats[mac] = ism;
				}

				string row = std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + " " + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";

				if ( all_stats[mac]["hourly"].find( row ) == all_stats[mac]["hourly"].end() )
				{
					InterfaceStats hstats;
					all_stats[mac]["hourly"][row] = hstats;
				}

				all_stats[mac]["hourly"][row].update( stats->tx_bytes, stats->rx_bytes );


				row.clear();
				row = std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );

				if ( all_stats[mac]["daily"].find( row ) == all_stats[mac]["daily"].end() )
				{
					InterfaceStats dstats;
					all_stats[mac]["daily"][row] = dstats;
				}

				all_stats[mac]["daily"][row].update( stats->tx_bytes, stats->rx_bytes );

				row.clear();
				row = std::to_string( y ) + "-" + std::to_string( m );

				if ( all_stats[mac]["monthly"].find( row ) == all_stats[mac]["monthly"].end() )
				{
					InterfaceStats mstats;
					all_stats[mac]["monthly"][row] = mstats;
				}

				all_stats[mac]["monthly"][row].update( stats->tx_bytes, stats->rx_bytes );


				row.clear();
				row = std::to_string( y );

				if ( all_stats[mac]["yearly"].find( row ) == all_stats[mac]["yearly"].end() )
				{
					InterfaceStats ystats;
					all_stats[mac]["yearly"][row] = ystats;
				}

				all_stats[mac]["yearly"][row].update( stats->tx_bytes, stats->rx_bytes );
				speed_stats[mac].update( stats->rx_bytes, stats->tx_bytes );

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

		freeifaddrs( ipa );
		cout << "\033[2J\033[1;1H";

		struct timeval te;
		gettimeofday( &te, NULL );
		uint64_t c_time = te.tv_sec * 1000LL + te.tv_usec / 1000;

		if ( c_time >= p_time + ( 1000 * save_interval ) )
		{
			string storage = settings["storage"];

			if ( Utils::contians(storage,"mysql") )
			{
#ifdef use_mysql
				//save_stats_to_mysql();
#endif // use_mysql
			}
			else if ( Utils::contians(storage,"sqlite") )
			{
#ifdef use_sqlite
				save_stats_to_sqlite();
#endif // use_sqlite
			}
			else if ( Utils::contians(storage,"files") )
			{
				save_stats_to_files();
			}

			p_time = c_time;
		}

		sleep( refresh_interval );
		k++;
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

int32_t mkpath( const std::string _s, mode_t mode )
{
	size_t pre = 0, pos;
	std::string dir;
	int32_t mdret = 0;
	string s( _s );

	if ( s[s.size() - 1] != '/' )
	{
		s += '/';
	}

	while ( ( pos = s.find_first_of( '/', pre ) ) != std::string::npos )
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
		string mac = mac_table.first;
		mkpath( mac, 0755 );

		const map<string, map<string, InterfaceStats> > & table = mac_table.second;

		for ( auto const & table_row : table )
		{
			string table_name = table_row.first;
			mkpath( mac + "/" + table_name, 0755 );

			const map<string, InterfaceStats> & row = table_row.second;

			for ( auto const & row_stats : row )
			{
				string row = row_stats.first;
				mkpath( mac + "/" + table_name + "/" + row, 0755 );

				const InterfaceStats& stats = row_stats.second;

				ofstream file;
				file.open( cwd + "/" + mac + "/" + table_name + "/" + row + "/stats.txt" );

				if ( file.is_open() == true )
				{
					uint64_t rx = stats.recieved();
					uint64_t tx = stats.transmited();

					//cout << mac + "/" + table_name + "/" + row + "\t" + std::to_string ( rx ) << endl;

					file << ( std::to_string( rx ) );
					file << endl;
					file << ( std::to_string( tx ) );

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

	for ( i = 0; i < argc; i++ )
	{
		printf( "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL" );
		table_columns[string( azColName[i] )] = string( argv[i] ? argv[i] : "0" );
	}

	//printf ( "\n" );
	return 0;
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
				query += std::to_string( rx );
				query += ",";
				query += std::to_string( tx );
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
				query += std::to_string( rx );
				query += ", ";
				query += "tx_bytes=";
				query += std::to_string( tx );
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

	for ( auto const & mac_table : all_stats )
	{
		sqlite3 *db;
		char *zErrMsg = 0;
		int rc;

		string mac = mac_table.first;
		const map<string, map<string, InterfaceStats> > & table = mac_table.second;

		rc = sqlite3_open_v2( ( cwd + "/" + mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );

		if ( rc != 0 )
		{
			continue;
		}

		for ( auto const & table_row : table )
		{
			string table_name = table_row.first;

			string query;
			query += "CREATE TABLE IF NOT EXISTS `" + table_name + "` (`row` VARCHAR(45) NULL,`rx_bytes` UNSIGNED BIGINT NULL,`tx_bytes` UNSIGNED BIGINT NULL,PRIMARY KEY (`row`));";

			rc = sqlite3_exec( db, query.c_str(), callback, 0, &zErrMsg );

			if ( rc != SQLITE_OK )
			{
				sqlite3_free( zErrMsg );
				printf( "can't create table" );
				continue;
			}

			const map<string, InterfaceStats> & row = table_row.second;

			for ( auto const & row_stats : row )
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
				rc = sqlite3_exec( db, query.c_str(), callback, 0, &zErrMsg );

				if ( rc != SQLITE_OK )
				{
					sqlite3_free( zErrMsg );
					printf( "error\n" );
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
					printf( "error\n" );
					continue;
				}
			}
		}
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

	map<string, InterfaceInfo> interfaces = Utils::get_all_interfaces();

	string row;
	ifstream file;

	for ( auto const & kv : interfaces )
	{
		const InterfaceInfo& in = kv.second;
		string mac = in.get_mac();

///
		get_time( &y, &m, &d, &h );

		row.clear();
		string row = std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + " " + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";
		file.open( cwd + "/" + mac + "/hourly/" + row + "/stats.txt", std::ifstream::in );

		if ( file.is_open() == true )
		{
			file >> rx_bytes;
			file >> tx_bytes;
			file.close();
			all_stats[mac]["hourly"][row].set_initial_stats( tx_bytes, rx_bytes );
		}

///
		get_time( &y, &m, &d, &h );

		row.clear();
		row = std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );
		file.open( cwd + "/" + mac + "/daily/" + row + "/stats.txt", std::ifstream::in );

		if ( file.is_open() == true )
		{
			file >> rx_bytes;
			file >> tx_bytes;
			file.close();
			all_stats[mac]["daily"][row].set_initial_stats( tx_bytes, rx_bytes );
		}

///
		get_time( &y, &m, &d, &h );

		row.clear();
		row = std::to_string( y ) + "-" + std::to_string( m );
		file.open( cwd + "/" + mac + "/monthly/" + row + "/stats.txt", std::ifstream::in );

		if ( file.is_open() == true )
		{
			file >> rx_bytes;
			file >> tx_bytes;
			file.close();
			all_stats[mac]["monthly"][row].set_initial_stats( tx_bytes, rx_bytes );
		}

///
		get_time( &y, &m, &d, &h );

		row.clear();
		row = std::to_string( y );
		file.open( cwd + "/" + mac + "/yearly/" + row + "/stats.txt", std::ifstream::in );

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

	map<string, InterfaceInfo> interfaces = Utils::get_all_interfaces();

	for ( auto const & kv : interfaces )
	{
		const InterfaceInfo& in = kv.second;
		string mac = in.get_mac();

		rc = sqlite3_open_v2( ( cwd + "/" + mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE, NULL );

		if ( rc != 0 )
		{
			continue;
		}

		get_time( &y, &m, &d, &h );

		string query = "SELECT * from yearly ";
		query += "WHERE row=";
		query += "'";
		query += std::to_string( y );
		query += "'";
		query += ";";
		table_columns.clear();
		rc = sqlite3_exec( db, query.c_str(), callback, NULL, &zErrMsg );

		uint64_t rx_bytes;
		uint64_t tx_bytes;
		string row = table_columns["row"];

		rx_bytes = std::stoull( table_columns["rx_bytes"] );

		tx_bytes = std::stoull( table_columns["tx_bytes"] );

		InterfaceStats ystats;
		all_stats[mac]["yearly"][row] = ystats;
		all_stats[mac]["yearly"][row].set_initial_stats( tx_bytes, rx_bytes );

///

		get_time( &y, &m, &d, &h );

		query.clear();
		query = "SELECT * from monthly ";
		query += "WHERE row=";
		query += "'";
		query += std::to_string( y ) + "-" + std::to_string( m );
		query += "'";
		query += ";";
		table_columns.clear();
		rc = sqlite3_exec( db, query.c_str(), callback, NULL, &zErrMsg );

		row = table_columns["row"];

		rx_bytes = std::stoull( table_columns["rx_bytes"] );

		tx_bytes = std::stoull( table_columns["tx_bytes"] );

		InterfaceStats mstats;
		all_stats[mac]["monthly"][row] = mstats;
		all_stats[mac]["monthly"][row].set_initial_stats( tx_bytes, rx_bytes );

///
		get_time( &y, &m, &d, &h );

		query.clear();
		query = "SELECT * from daily ";
		query += "WHERE row=";
		query += "'";
		query += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );
		query += "'";
		query += ";";

		table_columns.clear();
		rc = sqlite3_exec( db, query.c_str(), callback, NULL, &zErrMsg );

		row = table_columns["row"];
		rx_bytes = std::stoull( table_columns["rx_bytes"] );
		tx_bytes = std::stoull( table_columns["tx_bytes"] );

		InterfaceStats dstats;
		all_stats[mac]["daily"][row] = dstats;
		all_stats[mac]["daily"][row].set_initial_stats( tx_bytes, rx_bytes );

///
		get_time( &y, &m, &d, &h );

		query.clear();
		query = "SELECT * from hourly ";
		query += "WHERE row=";
		query += "'";
		query += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + " " + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";
		query += "'";
		query += ";";

		table_columns.clear();
		rc = sqlite3_exec( db, query.c_str(), callback, NULL, &zErrMsg );

		row = table_columns["row"];
		rx_bytes = std::stoull( table_columns["rx_bytes"] );
		tx_bytes = std::stoull( table_columns["tx_bytes"] );

		InterfaceStats hstats;
		all_stats[mac]["hourly"][row] = hstats;
		all_stats[mac]["hourly"][row].set_initial_stats( tx_bytes, rx_bytes );

		sqlite3_close( db );
	}

#endif // use_sqlite
}


static void signal_handler( int )
{
	//pthread_kill(t1, SIGTERM);
	//pthread_kill(t2, SIGTERM);

	string storage = settings["storage"];

	if ( Utils::contians(storage,"mysql") )
	{
#ifdef use_mysql
		//save_stats_to_mysql();
#endif // use_mysql
	}
	else if ( Utils::contians(storage,"sqlite") )
	{
#ifdef use_sqlite
		save_stats_to_sqlite();
#endif // use_sqlite
	}
	else if ( Utils::contians(storage,"files") )
	{
		save_stats_to_files();
	}

	exit( 0 );
}
