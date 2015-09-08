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

#include "Utils.h"

map<string, InterfaceInfo> Utils::get_all_interfaces( void )
{
	map<string, InterfaceInfo> interfaces;

	struct ifaddrs *ifaddr;
	int family, s;
	char host[NI_MAXHOST];

	if ( getifaddrs( &ifaddr ) == -1 )
	{
		perror( "getifaddrs" );
		exit( EXIT_FAILURE );
	}


	for ( ; ifaddr != NULL; ifaddr = ifaddr->ifa_next )
	{
		if ( ifaddr->ifa_addr == NULL || strcmp( ifaddr->ifa_name, "lo" ) == 0 )
		{
			continue;
		}

		family = ifaddr->ifa_addr->sa_family;

//        if (family != AF_INET && family != AF_INET6)
//            continue;

		/* For an AF_INET* interface address, display the address */

		if ( family == AF_INET ) //|| family == AF_INET6 )
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

			string interface_name = string( ifaddr->ifa_name );

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
			                 NULL, 0, NI_NUMERICHOST );

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

	freeifaddrs( ifaddr );

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

	delete out_buf;

	return mac;
}


