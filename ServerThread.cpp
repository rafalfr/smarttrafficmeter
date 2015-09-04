#include "ServerThread.h"
#include <map>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#include "json/json.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"

using namespace std;

extern map<string, map<string, map<string, InterfaceStats> > > all_stats;
extern map<string, InterfaceSpeedMeter> speed_stats;

//https://www.abc.se/~m6695/udp.html
//http://en.cppreference.com/w/cpp/links/libs
//http://jsoncpp.sourceforge.net/old.html

void* ServerThread::Thread( void* arg )
{
	int sockfd, n;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len;
	char mesg[256];

	sockfd = socket( AF_INET, SOCK_DGRAM, 0 );

	bzero( &servaddr, sizeof( servaddr ) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
	servaddr.sin_port = htons( 32000 );
	bind( sockfd, ( struct sockaddr * ) &servaddr, sizeof( servaddr ) );

	while ( true )
	{
		len = sizeof( cliaddr );
		memset( mesg, 0, 256 );
		n = recvfrom( sockfd, mesg, 256, 0, ( struct sockaddr * ) &cliaddr, &len );

		mesg[n] = 0;
		string command( mesg );

		if ( command.compare( "get speed" ) == 0 )
		{
			Json::Value root;
			Json::StyledWriter writer;

			for ( auto const & mac_speedinfo : speed_stats )
			{
				const string& mac = mac_speedinfo.first;
				const InterfaceSpeedMeter& ism = mac_speedinfo.second;

				root[mac]["down"] = Json::Value::UInt64(ism.get_rx_speed());
				root[mac]["up"] = Json::Value::UInt64(ism.get_tx_speed());
			}

			string response = writer.write( root );
			sendto( sockfd, response.c_str(), response.size(), 0, ( struct sockaddr * ) &cliaddr, sizeof( cliaddr ) );
		}
	}
}
