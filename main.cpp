#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <pthread.h>
#define _GNU_SOURCE     /* To get definition of NI_MAXHOST */
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
#include <net/if.h>#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "InterfaceInfo.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"

using namespace std;

//http://www.man7.org/tlpi/code/online/all_files_alpha.html
//http://man7.org/linux/man-pages/man3/getifaddrs.3.html
//http://stackoverflow.com/questions/1519585/how-to-get-mac-address-for-an-interface-in-linux-using-a-c-program
//http://stackoverflow.com/questions/18100097/portable-way-to-check-if-directory-exists-windows-linux-c
//http://stackoverflow.com/questions/675039/how-can-i-create-directory-tree-in-c-linux

uint32_t refresh_interval = 1;  //statistics interval in seconds
uint32_t save_interval = 5;     //save interval in seconds

map<string, map<string, map<string, InterfaceStats> > > all_stats;
map<string, InterfaceSpeedMeter> speed_stats;
map<string, string> table_columns;



map<string, InterfaceInfo> get_all_interfaces ( void );
void print_hourly_stats ( const map<string, map<uint32_t, InterfaceStats>>& stats );
void print_daily_stats ( const map<string, map<uint32_t, InterfaceStats>>& stats );
void print_monthly_stats ( const map<string, map<uint32_t, InterfaceStats>>& stats );
void print_yearly_stats ( const map<string, map<uint32_t, InterfaceStats>>& stats );
void save_stats_to_sqlite ( void );
void load_data_from_sqlite ( void );
int dirExists ( const char *path );
int mkpath ( std::string s, mode_t mode );
static void signal_handler ( int );
static void * MeterThread ( void *arg );
void get_time ( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h );

int main()
{
	pthread_t t1;
	void *res;
	int s;

	signal ( SIGINT, signal_handler );
	signal ( SIGSEGV, signal_handler );
	signal ( SIGTERM, signal_handler );

	load_data_from_sqlite();

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

	map<string, InterfaceInfo> interfaces = get_all_interfaces();

	for ( auto const & mac_info : interfaces )
	{
		string mac = mac_info.second.get_mac();

		InterfaceSpeedMeter ism;
		speed_stats[mac]=ism;

		if ( all_stats.find ( mac ) == all_stats.end() )
		{
			get_time ( &y, &m, &d, &h );

			InterfaceStats hstats;
			string row = std::to_string ( y ) + "-" + std::to_string ( m ) + "-" + std::to_string ( d ) + " " + std::to_string ( h ) + ":00-" + std::to_string ( h + 1 ) + ":00";
			all_stats[mac]["hourly"][row] = hstats;

			InterfaceStats dstats;
			row.clear();
			row = std::to_string ( y ) + "-" + std::to_string ( m ) + "-" + std::to_string ( d );
			all_stats[mac]["daily"][row] = dstats;

			InterfaceStats mstats;
			row.clear();
			row = std::to_string ( y ) + "-" + std::to_string ( m );
			all_stats[mac]["monthly"][row] = mstats;

			InterfaceStats ystats;
			row.clear();
			row = std::to_string ( y );
			all_stats[mac]["yearly"][row] = ystats;
		}
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


	s = pthread_create ( &t1, NULL, &MeterThread, NULL );
	if ( s != 0 ) {
		return 1;
	}

	s = pthread_join ( t1, &res );
	if ( s != 0 ) {
		return 1;
	}

	exit ( EXIT_SUCCESS );
}

string get_mac ( char* name )
{
	int s;
	struct ifreq buffer;
	char* out_buf;
	string mac;

	out_buf = new char[256];
	memset ( out_buf, 0x00, 256 * sizeof ( char ) );

	s = socket ( PF_INET, SOCK_DGRAM, 0 );

	memset ( &buffer, 0x00, sizeof ( buffer ) );

	strcpy ( buffer.ifr_name, name );

	ioctl ( s, SIOCGIFHWADDR, &buffer );

	close ( s );

	for ( s = 0; s < 6; s++ )
	{
		sprintf ( out_buf, "%.2x-%.2x-%.2x-%.2x-%.2x-%.2x", ( unsigned char ) buffer.ifr_hwaddr.sa_data[0], ( unsigned char ) buffer.ifr_hwaddr.sa_data[1], ( unsigned char ) buffer.ifr_hwaddr.sa_data[2], ( unsigned char ) buffer.ifr_hwaddr.sa_data[3], ( unsigned char ) buffer.ifr_hwaddr.sa_data[4], ( unsigned char ) buffer.ifr_hwaddr.sa_data[5] );
	}

	mac.clear();
	mac.append ( out_buf );

	delete out_buf;

	return mac;
}

map<string, InterfaceInfo> get_all_interfaces ( void )
{
	//vector<InterfaceInfo> interfaces;

	map<string, InterfaceInfo> interfaces;

	struct ifaddrs *ifaddr;
	int family, s;
	char host[NI_MAXHOST];

	if ( getifaddrs ( &ifaddr ) == -1 )
	{
		perror ( "getifaddrs" );
		exit ( EXIT_FAILURE );
	}


	for ( ; ifaddr != NULL; ifaddr = ifaddr->ifa_next )
	{
		if ( ifaddr->ifa_addr == NULL || strcmp ( ifaddr->ifa_name, "lo" ) == 0 )
		{
			continue;
		}

		family = ifaddr->ifa_addr->sa_family;

//        if (family != AF_INET && family != AF_INET6)
//            continue;

		/* For an AF_INET* interface address, display the address */

		if ( family == AF_INET ) //|| family == AF_INET6 )
		{
			s = getnameinfo ( ifaddr->ifa_addr,
							  ( family == AF_INET ) ? sizeof ( struct sockaddr_in ) :
							  sizeof ( struct sockaddr_in6 ),
							  host, NI_MAXHOST,
							  NULL, 0, NI_NUMERICHOST );
			if ( s != 0 )
			{
				continue;
			}

			string interface_name = string ( ifaddr->ifa_name );
			if ( interfaces.find ( interface_name ) == interfaces.end() )
			{
				InterfaceInfo in;
				in.set_name ( ifaddr->ifa_name );
				in.set_mac ( get_mac ( ifaddr->ifa_name ).c_str() );
				in.set_ip4 ( host );
				interfaces[interface_name] = in;
			}
			else
			{
				interfaces[interface_name].set_ip4 ( host );
			}
		}
		else if ( family == AF_INET6 )
		{
			s = getnameinfo ( ifaddr->ifa_addr,
							  ( family == AF_INET ) ? sizeof ( struct sockaddr_in ) :
							  sizeof ( struct sockaddr_in6 ),
							  host, NI_MAXHOST,
							  NULL, 0, NI_NUMERICHOST );
			if ( s != 0 )
			{
				continue;
			}

			string interface_name = string ( ifaddr->ifa_name );
			if ( interfaces.find ( interface_name ) == interfaces.end() )
			{
				InterfaceInfo in;
				in.set_name ( ifaddr->ifa_name );
				in.set_mac ( get_mac ( ifaddr->ifa_name ).c_str() );
				in.set_ip6 ( host );
				interfaces[interface_name] = in;
			}
			else
			{
				interfaces[interface_name].set_ip6 ( host );
			}
		}
	}
	freeifaddrs ( ifaddr );

	return interfaces;
}

static void * MeterThread ( void *arg )
{

	static uint64_t p_time = 0;
	struct ifaddrs *ifaddr;
	int family, s;
	char host[NI_MAXHOST];
	string hourly ( "hourly" );
	string daily ( "daily" );
	string monthly ( "monthly" );
	string yearly ( "yearly" );


	while ( true )
	{

		if ( getifaddrs ( &ifaddr ) == -1 )
		{
			continue;
		}

		for ( ; ifaddr != NULL; ifaddr = ifaddr->ifa_next )
		{

			if ( ifaddr->ifa_addr == NULL || strcmp ( ifaddr->ifa_name, "lo" ) == 0 ) {
				continue;
			}

			family = ifaddr->ifa_addr->sa_family;


			/* For an AF_INET* interface address, display the address */

			if ( family == AF_INET || family == AF_INET6 )
			{
				s = getnameinfo ( ifaddr->ifa_addr,
								  ( family == AF_INET ) ? sizeof ( struct sockaddr_in ) :
								  sizeof ( struct sockaddr_in6 ),
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

				printf ( "\t\ttx_packets = %10u; rx_packets = %10u\n"
						 "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n",
						 stats->tx_packets, stats->rx_packets,
						 stats->tx_bytes, stats->rx_bytes );

				uint32_t y;
				uint32_t m;
				uint32_t d;
				uint32_t h;
				get_time ( &y, &m, &d, &h );

				string mac = get_mac ( ifaddr->ifa_name ).c_str();

				if (speed_stats.find(mac)==speed_stats.end())
                {
                    InterfaceSpeedMeter ism;
                    speed_stats[mac]=ism;
                }

				string row = std::to_string ( y ) + "-" + std::to_string ( m ) + "-" + std::to_string ( d ) + " " + std::to_string ( h ) + ":00-" + std::to_string ( h + 1 ) + ":00";

				if ( all_stats[mac]["hourly"].find ( row ) == all_stats[mac]["hourly"].end() )
				{
					InterfaceStats hstats;
					all_stats[mac]["hourly"][row] = hstats;
				}
				all_stats[mac]["hourly"][row].update ( stats->tx_bytes, stats->rx_bytes );


				row.clear();
				row = std::to_string ( y ) + "-" + std::to_string ( m ) + "-" + std::to_string ( d );

				if ( all_stats[mac]["daily"].find ( row ) == all_stats[mac]["daily"].end() )
				{
					InterfaceStats dstats;
					all_stats[mac]["daily"][row] = dstats;
				}
				all_stats[mac]["daily"][row].update ( stats->tx_bytes, stats->rx_bytes );

				row.clear();
				row = std::to_string ( y ) + "-" + std::to_string ( m );

				if ( all_stats[mac]["monthly"].find ( row ) == all_stats[mac]["monthly"].end() )
				{
					InterfaceStats mstats;
					all_stats[mac]["monthly"][row] = mstats;
				}
				all_stats[mac]["monthly"][row].update ( stats->tx_bytes, stats->rx_bytes );


				row.clear();
				row = std::to_string ( y );

				if ( all_stats[mac]["yearly"].find ( row ) == all_stats[mac]["yearly"].end() )
				{
					InterfaceStats ystats;
					all_stats[mac]["yearly"][row] = ystats;
				}
				all_stats[mac]["yearly"][row].update ( stats->tx_bytes, stats->rx_bytes );
				speed_stats[mac].update(stats->rx_bytes,stats->tx_bytes);

				for(auto const & mac_speedinfo : speed_stats)
                {
                    const string& mac=mac_speedinfo.first;
                    InterfaceSpeedMeter& ism=mac_speedinfo.second;
                    cout<<mac<<endl;
                    //cout<<mac<<endl<<"up: "<<ism.get_tx_speed()<<"\tdown: "<<ism.get_rx_speed()<<endl;
                    printf("%ld",ism.get_tx_speed());
                }


//				uint32_t key = ( y << 14 ) | ( m << 10 ) | ( d << 5 ) | h;
//				if ( hourly_stats[mac].find ( key ) == hourly_stats[mac].end() )
//				{
//					InterfaceStats interface_stats;
//					hourly_stats[mac][key] = interface_stats;
//					hourly_stats[mac][key].update ( stats->tx_bytes, stats->rx_bytes );
//				}
//				else
//				{
//					hourly_stats[mac][key].update ( stats->tx_bytes, stats->rx_bytes );
//				}
//
//				key = ( y << 9 ) | ( m << 5 ) | d;
//				if ( daily_stats[mac].find ( key ) == daily_stats[mac].end() )
//				{
//					InterfaceStats interface_stats;
//					daily_stats[mac][key] = interface_stats;
//					daily_stats[mac][key].update ( stats->tx_bytes, stats->rx_bytes );
//				}
//				else
//				{
//					daily_stats[mac][key].update ( stats->tx_bytes, stats->rx_bytes );
//				}
//
//				key = ( y << 4 ) | m;
//				if ( monthly_stats[mac].find ( key ) == monthly_stats[mac].end() )
//				{
//					InterfaceStats interface_stats;
//					monthly_stats[mac][key] = interface_stats;
//					monthly_stats[mac][key].update ( stats->tx_bytes, stats->rx_bytes );
//				}
//				else
//				{
//					monthly_stats[mac][key].update ( stats->tx_bytes, stats->rx_bytes );
//				}
//
//				key = y;
//				if ( yearly_stats[mac].find ( key ) == yearly_stats[mac].end() )
//				{
//					InterfaceStats interface_stats;
//					yearly_stats[mac][key] = interface_stats;
//					yearly_stats[mac][key].update ( stats->tx_bytes, stats->rx_bytes );
//				}
//				else
//				{
//					yearly_stats[mac][key].update ( stats->tx_bytes, stats->rx_bytes );
//				}
			}
		}
		freeifaddrs ( ifaddr );
		//cout << "\033[2J\033[1;1H";
		//print_hourly_stats ( hourly_stats );
		//print_daily_stats ( daily_stats );
		//print_monthly_stats ( monthly_stats );
		//print_yearly_stats ( yearly_stats );

		struct timeval te;
		gettimeofday ( &te, NULL );
		uint64_t c_time = te.tv_sec * 1000LL + te.tv_usec / 1000;
		if ( c_time >= p_time + ( 1000 * save_interval ) )
		{
			save_stats_to_sqlite();
			p_time = c_time;
		}
		sleep ( refresh_interval );
	}
}

int dirExists ( const char *path )
{
	struct stat info;

	if ( stat ( path, &info ) != 0 ) {
		return 0;
	}
	else if ( info.st_mode & S_IFDIR ) {
		return 1;
	}
	else {
		return 0;
	}
}

int mkpath ( std::string s, mode_t mode )
{
	size_t pre = 0, pos;
	std::string dir;
	int mdret;

	if ( s[s.size() - 1] != '/' ) {
		s += '/';
	}

	while ( ( pos = s.find_first_of ( '/', pre ) ) != std::string::npos ) {
		dir = s.substr ( 0, pos++ );
		pre = pos;
		if ( dir.size() == 0 ) {
			continue;    // if leading / first time is 0 length
		}
		if ( ( mdret = mkdir ( dir.c_str(), mode ) ) && errno != EEXIST ) {
			return mdret;
		}
	}
	return mdret;
}

void get_time ( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h )
{
	time_t t = time ( NULL );
	struct tm* tm = localtime ( &t );
	( *y ) = tm->tm_year + 1900;
	( *m ) = tm->tm_mon + 1;
	( *d ) = tm->tm_mday;
	( *h ) = tm->tm_hour;
}

void print_hourly_stats ( const map<string, map<uint32_t, InterfaceStats>>& stats )
{
	for ( auto const & kv : stats )
	{
		string mac = kv.first;
		cout << mac << ":" << endl;
		map<uint32_t, InterfaceStats> interface_stats = kv.second;
		for ( auto const & kv : interface_stats )
		{
			uint32_t key = kv.first;
			InterfaceStats s = kv.second;
			key = key & 0x1F;
			cout << key << "\trecieved=" << s.recieved() << "\ttransmited=" << s.transmited() << endl;
		}
	}
}

void print_daily_stats ( const map<string, map<uint32_t, InterfaceStats>>& stats )
{
	for ( auto const & kv : stats )
	{
		string mac = kv.first;
		cout << mac << ":" << endl;
		map<uint32_t, InterfaceStats> interface_stats = kv.second;
		for ( auto const & kv : interface_stats )
		{
			uint32_t key = kv.first;
			InterfaceStats s = kv.second;
			key = key & 0x1F;
			cout << key << "\trecieved=" << s.recieved() << "\ttransmited=" << s.transmited() << endl;
		}
	}
}

void print_monthly_stats ( const map<string, map<uint32_t, InterfaceStats>>& stats )
{
	for ( auto const & kv : stats )
	{
		string mac = kv.first;
		cout << mac << ":" << endl;
		map<uint32_t, InterfaceStats> interface_stats = kv.second;
		for ( auto const & kv : interface_stats )
		{
			uint32_t key = kv.first;
			InterfaceStats s = kv.second;
			key = key & 0xF;
			cout << key << "\trecieved=" << s.recieved() << "\ttransmited=" << s.transmited() << endl;
		}
	}
}

void print_yearly_stats ( const map<string, map<uint32_t, InterfaceStats>>& stats )
{
	for ( auto const & kv : stats )
	{
		string mac = kv.first;
		cout << mac << ":" << endl;
		map<uint32_t, InterfaceStats> interface_stats = kv.second;
		for ( auto const & kv : interface_stats )
		{
			uint32_t key = kv.first;
			InterfaceStats s = kv.second;
			cout << key << "\trecieved=" << s.recieved() << "\ttransmited=" << s.transmited() << endl;
		}
	}
}

static int callback ( void *NotUsed, int argc, char **argv, char **azColName )
{
	int i;
	for ( i = 0; i < argc; i++ ) {
		printf ( "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL" );
		table_columns[string ( azColName[i] )] = string ( argv[i] ? argv[i] : "0" );
	}
	//printf ( "\n" );
	return 0;
}

void save_stats_to_sqlite ( void )
{
//	string path;
//	for ( auto const & kv : yearly_stats )
//	{
//		string mac = kv.first;
//		map<uint32_t, InterfaceStats> ystats = kv.second;
//		for ( auto const & kv : ystats )
//		{
//			uint32_t year = kv.first;
//			InterfaceStats s = kv.second;
//			path = mac + "/" + std::to_string ( year ) + "/";
//			mkpath ( path, 0755 );
//
//
//			map<uint32_t, InterfaceStats> mstats = monthly_stats[mac];
//			for ( auto const & kv : mstats )
//			{
//				uint32_t month = kv.first & 0xF;
//				path += std::to_string ( month ) + "/";
//				mkpath ( path, 0755 );
//
//				map<uint32_t, InterfaceStats> dstats = daily_stats[mac];
//				for ( auto const & kv : dstats )
//				{
//					uint32_t day = kv.first & 0x1F;
//					path += std::to_string ( day ) + "/";
//					mkpath ( path, 0755 );
//
//					map<uint32_t, InterfaceStats> hstats = hourly_stats[mac];
//					for ( auto const & kv : hstats )
//					{
//						uint32_t hour = kv.first & 0x1F;
//						path += std::to_string ( hour ) + "/";
//						mkpath ( path, 0755 );
//					}
//				}
//			}
//		}
//	}

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

		rc = sqlite3_open_v2 ( ( mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
		if ( rc != 0 )
		{
			continue;
		}

		for ( auto const & table_row : table )
		{
			string table_name = table_row.first;

			string query;
			query += "CREATE TABLE IF NOT EXISTS `" + table_name + "` (`row` VARCHAR(45) NULL,`rx_bytes` BIGINT NULL,`tx_bytes` BIGINT NULL,PRIMARY KEY (`row`));";

			rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
			if ( rc != SQLITE_OK ) {
				sqlite3_free ( zErrMsg );
				printf ( "can't create table" );
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
				query += std::to_string ( rx );
				query += ",";
				query += std::to_string ( tx );
				query += ");";
				rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
				if ( rc != SQLITE_OK ) {
					sqlite3_free ( zErrMsg );
					printf ( "error\n" );
					continue;
				}

				query.clear();
				query += "UPDATE OR IGNORE " + table_name + " SET ";
				query += "rx_bytes=";
				query += std::to_string ( rx );
				query += ", ";
				query += "tx_bytes=";
				query += std::to_string ( tx );
				query += " WHERE row='" + row + "'";
				query += ";";
				rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
				if ( rc != SQLITE_OK ) {
					sqlite3_free ( zErrMsg );
					printf ( "error\n" );
					continue;
				}
			}
		}
	}


//	for ( auto const & kv : yearly_stats )
//	{
//		sqlite3 *db;
//		char *zErrMsg = 0;
//		int rc;
//
//		string mac = kv.first;
//
//		rc = sqlite3_open_v2 ( ( mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
//		if ( rc != 0 )
//		{
//			continue;
//		}
//
//		string query;
//		query += "CREATE TABLE IF NOT EXISTS `YEARLY` (`year` VARCHAR(45) NULL,`rx_bytes` BIGINT NULL,`tx_bytes` BIGINT NULL,PRIMARY KEY (`year`));";
//
//		rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
//		if ( rc != SQLITE_OK ) {
//			sqlite3_free ( zErrMsg );
//			printf ( "can't open the database" );
//			continue;
//		}
//
//		map<uint32_t, InterfaceStats> ystats = kv.second;
//		for ( auto const & kv : ystats )
//		{
//			uint32_t year = kv.first;
//			InterfaceStats s = kv.second;
//			uint64_t rx = s.recieved();
//			uint64_t tx = s.transmited();
//			string query;
//			query += "INSERT OR IGNORE INTO YEARLY (year,rx_bytes,tx_bytes) VALUES(";
//			query += "'";
//			query += std::to_string ( year );
//			query += "'";
//			query += ",";
//			query += std::to_string ( rx );
//			query += ",";
//			query += std::to_string ( tx );
//			query += ");";
//			rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
//			if ( rc != SQLITE_OK ) {
//				sqlite3_free ( zErrMsg );
//				printf ( "error\n" );
//				continue;
//			}
//
//			query.clear();
//			query += "UPDATE OR IGNORE YEARLY SET ";
//			query += "rx_bytes=";
//			query += std::to_string ( rx );
//			query += ", ";
//			query += "tx_bytes=";
//			query += std::to_string ( tx );
//			query += " WHERE year='" + std::to_string ( year ) + "'";
//			query += ";";
//			rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
//			if ( rc != SQLITE_OK ) {
//				sqlite3_free ( zErrMsg );
//				printf ( "error\n" );
//				continue;
//			}
//		}
//		sqlite3_close ( db );
//	}
//
//////////////////////////////////////////
//	for ( auto const & kv : monthly_stats )
//	{
//		sqlite3 *db;
//		char *zErrMsg = 0;
//		int rc;
//
//		string mac = kv.first;
//
//		rc = sqlite3_open_v2 ( ( mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
//		if ( rc != 0 )
//		{
//			continue;
//		}
//
//		string query;
//		query += "CREATE TABLE IF NOT EXISTS `MONTHLY` (`month` VARCHAR(45) NULL,`rx_bytes` BIGINT NULL,`tx_bytes` BIGINT NULL,PRIMARY KEY (`month`));";
//
//		rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
//		if ( rc != SQLITE_OK ) {
//			sqlite3_free ( zErrMsg );
//			printf ( "can't create the table" );
//			continue;
//		}
//
//		map<uint32_t, InterfaceStats> mstats = kv.second;
//		for ( auto const & kv : mstats )
//		{
//			uint32_t key = kv.first;
//			uint32_t year = ( key >> 4 ) & 0xFFFF;
//			uint32_t month = key & 0xF;
//			InterfaceStats s = kv.second;
//			uint64_t rx = s.recieved();
//			uint64_t tx = s.transmited();
//			string query;
//			query += "INSERT OR IGNORE INTO MONTHLY (month,rx_bytes,tx_bytes) VALUES(";
//			query += "'";
//			query += std::to_string ( year ) + "-" + std::to_string ( month );
//			query += "'";
//			query += ",";
//			query += std::to_string ( rx );
//			query += ",";
//			query += std::to_string ( tx );
//			query += ");";
//			rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
//			if ( rc != SQLITE_OK ) {
//				sqlite3_free ( zErrMsg );
//				printf ( "error\n" );
//				continue;
//			}
//
//			query.clear();
//			query += "UPDATE OR IGNORE MONTHLY SET ";
//			query += "rx_bytes=";
//			query += std::to_string ( rx );
//			query += ", ";
//			query += "tx_bytes=";
//			query += std::to_string ( tx );
//			query += " WHERE month='" + std::to_string ( year ) + "-" + std::to_string ( month ) + "'";
//			query += ";";
//			rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
//			if ( rc != SQLITE_OK ) {
//				sqlite3_free ( zErrMsg );
//				printf ( "error\n" );
//				continue;
//			}
//		}
//		sqlite3_close ( db );
//	}
//
///////////////////////////////////////////////
//	for ( auto const & kv : daily_stats )
//	{
//		sqlite3 *db;
//		char *zErrMsg = 0;
//		int rc;
//
//		string mac = kv.first;
//
//		rc = sqlite3_open_v2 ( ( mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
//		if ( rc != 0 )
//		{
//			continue;
//		}
//
//		string query;
//		query += "CREATE TABLE IF NOT EXISTS `DAILY` (`day` VARCHAR(45) NULL,`rx_bytes` BIGINT NULL,`tx_bytes` BIGINT NULL,PRIMARY KEY (`day`));";
//
//		rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
//		if ( rc != SQLITE_OK ) {
//			sqlite3_free ( zErrMsg );
//			printf ( "can't create the table" );
//			continue;
//		}
//
//		map<uint32_t, InterfaceStats> dstats = kv.second;
//		for ( auto const & kv : dstats )
//		{
//			uint32_t key = kv.first;
//			uint32_t year = ( key >> 9 ) & 0xFFFF;
//			uint32_t month = ( key >> 5 ) & 0xF;
//			uint32_t day = key & 0x1F;
//			InterfaceStats s = kv.second;
//			uint64_t rx = s.recieved();
//			uint64_t tx = s.transmited();
//			string query;
//			query += "INSERT OR IGNORE INTO DAILY (day,rx_bytes,tx_bytes) VALUES(";
//			query += "'";
//			query += std::to_string ( year ) + "-" + std::to_string ( month ) + "-" + std::to_string ( day );
//			query += "'";
//			query += ",";
//			query += std::to_string ( rx );
//			query += ",";
//			query += std::to_string ( tx );
//			query += ");";
//			rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
//			if ( rc != SQLITE_OK ) {
//				sqlite3_free ( zErrMsg );
//				printf ( "error\n" );
//				continue;
//			}
//
//			query.clear();
//			query += "UPDATE OR IGNORE DAILY SET ";
//			query += "rx_bytes=";
//			query += std::to_string ( rx );
//			query += ", ";
//			query += "tx_bytes=";
//			query += std::to_string ( tx );
//			query += " WHERE day='" + std::to_string ( year ) + "-" + std::to_string ( month ) + "-" + std::to_string ( day ) + "'";
//			query += ";";
//			rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
//			if ( rc != SQLITE_OK ) {
//				sqlite3_free ( zErrMsg );
//				printf ( "error\n" );
//				continue;
//			}
//		}
//		sqlite3_close ( db );
//	}
//
////////////////
//	for ( auto const & kv : hourly_stats )
//	{
//		sqlite3 *db;
//		char *zErrMsg = 0;
//		int rc;
//
//		string mac = kv.first;
//
//		rc = sqlite3_open_v2 ( ( mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
//		if ( rc != 0 )
//		{
//			continue;
//		}
//
//		string query;
//		query += "CREATE TABLE IF NOT EXISTS `HOURLY` (`hour` VARCHAR(45) NULL,`rx_bytes` BIGINT NULL,`tx_bytes` BIGINT NULL,PRIMARY KEY (`hour`));";
//
//		rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
//		if ( rc != SQLITE_OK ) {
//			sqlite3_free ( zErrMsg );
//			printf ( "can't create the table" );
//			continue;
//		}
//
//		map<uint32_t, InterfaceStats> dstats = kv.second;
//		for ( auto const & kv : dstats )
//		{
//			uint32_t key = kv.first;
//			uint32_t year = ( key >> 14 ) & 0xFFFF;
//			uint32_t month = ( key >> 10 ) & 0xF;
//			uint32_t day = ( key >> 5 ) & 0x1F;
//			uint32_t hour = key & 0x1F;
//			InterfaceStats s = kv.second;
//			uint64_t rx = s.recieved();
//			uint64_t tx = s.transmited();
//			string query;
//			query += "INSERT OR IGNORE INTO HOURLY (hour,rx_bytes,tx_bytes) VALUES(";
//			query += "'";
//			query += std::to_string ( year ) + "-" + std::to_string ( month ) + "-" + std::to_string ( day ) + "-" + std::to_string ( hour );
//			query += "'";
//			query += ",";
//			query += std::to_string ( rx );
//			query += ",";
//			query += std::to_string ( tx );
//			query += ");";
//			cout << query << endl;
//			rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
//			if ( rc != SQLITE_OK ) {
//				sqlite3_free ( zErrMsg );
//				printf ( "error\n" );
//				continue;
//			}
//
//			query.clear();
//			query += "UPDATE OR IGNORE HOURLY SET ";
//			query += "rx_bytes=";
//			query += std::to_string ( rx );
//			query += ", ";
//			query += "tx_bytes=";
//			query += std::to_string ( tx );
//			query += " WHERE hour='" + std::to_string ( year ) + "-" + std::to_string ( month ) + "-" + std::to_string ( day ) + "-" + std::to_string ( hour ) + "'";
//			query += ";";
//			rc = sqlite3_exec ( db, query.c_str(), callback, 0, &zErrMsg );
//			if ( rc != SQLITE_OK ) {
//				sqlite3_free ( zErrMsg );
//				printf ( "error\n" );
//				continue;
//			}
//		}
//		sqlite3_close ( db );
//	}

}

void load_data_from_sqlite ( void )
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	uint32_t y;
	uint32_t m;
	uint32_t d;
	uint32_t h;

	map<string, InterfaceInfo> interfaces = get_all_interfaces();

	for ( auto const & kv : interfaces )
	{
		const InterfaceInfo& in = kv.second;
		string mac = in.get_mac();

		rc = sqlite3_open_v2 ( ( mac + ".db" ).c_str(), &db, SQLITE_OPEN_READWRITE, NULL );
		if ( rc != 0 )
		{
			continue;
		}

		get_time ( &y, &m, &d, &h );

		string query = "SELECT * from yearly ";
		query += "WHERE row=";
		query += "'";
		query += std::to_string ( y );
		query += "'";
		query += ";";
		table_columns.clear();
		rc = sqlite3_exec ( db, query.c_str(), callback, NULL, &zErrMsg );

		uint64_t rx_bytes;
		uint64_t tx_bytes;
		string row = table_columns["row"];

		rx_bytes = std::stoull ( table_columns["rx_bytes"] );

		tx_bytes = std::stoull ( table_columns["tx_bytes"] );

		InterfaceStats ystats;
		all_stats[mac]["yearly"][row] = ystats;
		all_stats[mac]["yearly"][row].update ( tx_bytes, rx_bytes );

///

		get_time ( &y, &m, &d, &h );

		query.clear();
		query = "SELECT * from monthly ";
		query += "WHERE row=";
		query += "'";
		query += std::to_string ( y ) + "-" + std::to_string ( m );
		query += "'";
		query += ";";
		table_columns.clear();
		rc = sqlite3_exec ( db, query.c_str(), callback, NULL, &zErrMsg );

		row = table_columns["row"];

		rx_bytes = std::stoull ( table_columns["rx_bytes"] );

		tx_bytes = std::stoull ( table_columns["tx_bytes"] );

		InterfaceStats mstats;
		all_stats[mac]["monthly"][row] = mstats;
		all_stats[mac]["monthly"][row].update ( tx_bytes, rx_bytes );

///
		get_time ( &y, &m, &d, &h );

		query.clear();
		query = "SELECT * from daily ";
		query += "WHERE row=";
		query += "'";
		query += std::to_string ( y ) + "-" + std::to_string ( m ) + "-" + std::to_string ( d );
		query += "'";
		query += ";";

		table_columns.clear();
		rc = sqlite3_exec ( db, query.c_str(), callback, NULL, &zErrMsg );

		row = table_columns["row"];
		rx_bytes = std::stoull ( table_columns["rx_bytes"] );
		tx_bytes = std::stoull ( table_columns["tx_bytes"] );

		InterfaceStats dstats;
		all_stats[mac]["daily"][row] = dstats;
		all_stats[mac]["daily"][row].update ( tx_bytes, rx_bytes );

///
		get_time ( &y, &m, &d, &h );

		query.clear();
		query = "SELECT * from hourly ";
		query += "WHERE row=";
		query += "'";
		query += std::to_string ( y ) + "-" + std::to_string ( m ) + "-" + std::to_string ( d ) + " " + std::to_string ( h ) + ":00-" + std::to_string ( h + 1 ) + ":00";
		query += "'";
		query += ";";

		table_columns.clear();
		rc = sqlite3_exec ( db, query.c_str(), callback, NULL, &zErrMsg );

		row = table_columns["row"];
		rx_bytes = std::stoull ( table_columns["rx_bytes"] );
		tx_bytes = std::stoull ( table_columns["tx_bytes"] );

		InterfaceStats hstats;
		all_stats[mac]["hourly"][row] = hstats;
		all_stats[mac]["hourly"][row].update ( tx_bytes, rx_bytes );

		sqlite3_close ( db );
	}

}


static void signal_handler ( int )
{
	exit ( 0 );
}
