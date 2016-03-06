#include <algorithm>
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
#include <sys/file.h>
#include <errno.h>

#include "Utils.h"

map<string, InterfaceInfo> Utils::get_all_interfaces( void )
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

            string interface_name( ifaddr->ifa_name );

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

    delete []host;

    freeifaddrs( ipa );

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

    delete []out_buf;

    return mac;
}

void Utils::get_time( uint32_t* y, uint32_t* m, uint32_t* d, uint32_t* h )
{
    time_t t = time( NULL );
    struct tm* tm = localtime( &t );
    ( *y ) = tm->tm_year + 1900;
    ( *m ) = tm->tm_mon + 1;
    ( *d ) = tm->tm_mday;
    ( *h ) = tm->tm_hour;
}
/** @brief contians
  *
  * the function returns true if key is found in the str
  *
  */
bool Utils::contians( const string& str, const string& key )
{
    if ( str.find( key ) != string::npos )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/** @brief starts_with
  *
  *function returns true if str starts with key
  *
  */
bool Utils::starts_with( const string& str, const string& key )
{
    if ( str.find( key ) == 0 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/** @brief split
  *
  * @todo: document this function
  */
vector<string> Utils::split( const string& str, const string& delim )
{
//http://garajeando.blogspot.com/2014/03/using-c11-to-split-strings-without.html
//http://www.informit.com/articles/article.aspx?p=2064649&seqNum=

    size_t start;
    size_t end;

    vector<string> items;

    start = 0;
    end = str.find( delim );

    while ( end != string::npos )
    {
        items.push_back( str.substr( start, end - start ) );
        start = end + delim.length();
        end = str.find( delim, start );
    }

    items.push_back( str.substr( start ) );

    return items;
}

/** @brief replace
  *
  * @todo: document this function
  */
void Utils::replace( const string& pattern, const string& with, const string& in )
{
    size_t pos = 0;

    while ( ( pos = in.find( pattern, pos ) ) != string::npos )
    {
		in.replace(pos,with.size(),with);
    }
}


/** @brief to_string
  *
  * @todo: document this function
  */
string Utils::to_string( uint64_t value )
{
    static uint64_t maxd = 1ULL;

    if ( maxd == 1ULL )
    {
        //compute maximum decimal value
        for ( uint64_t i = 0ULL; i < sizeof( uint64_t ) * 8ULL; i++ )
        {
            uint64_t v = 1ULL << i;

            if ( ( v / maxd ) != 0ULL )
            {
                maxd *= 10ULL;
            }
        }
    }

    string out;

    uint64_t divisor = maxd;

    if ( value == 0ULL )
    {
        out += "0";
    }
    else
    {
        while ( divisor > 0 )
        {
            uint64_t c = value / divisor;
            value -= c * divisor;
            divisor /= 10ULL;

            if ( out.empty() == false || c > 0ULL )
            {
                out += std::to_string( c );
            }
        }
    }

    return out;
}

/** @brief check_one_instance
  *
  * @todo: document this function
  */
bool Utils::check_one_instance( void )
{
    int pid_file = open( "smarttrafficmeter.pid", O_CREAT | O_RDWR, 0666 );
    int rc = flock( pid_file, LOCK_EX | LOCK_NB );

    if ( rc != 0 && ( errno == EWOULDBLOCK ) )
    {
        return false;
    }

    return true;
}

/** @brief trim
  *
  * @todo: document this function
  */
string Utils::trim( const std::string& s )
{
    //http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

    auto wsfront = std::find_if_not( s.begin(), s.end(), []( int c )
    {
        return std::isspace( c );
    } );

    auto wsback = std::find_if_not( s.rbegin(), s.rend(), []( int c )
    {
        return std::isspace( c );
    } ).base();

    return ( wsback <= wsfront ? std::string() : std::string( wsfront, wsback ) );
}




