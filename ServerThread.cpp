#include "ServerThread.h"
#include <map>
#include <string>
#include <cstring>
#include <stdio.h>

#include <boost/asio.hpp>

#include "json/json.h"
#include "Utils.h"
#include "Globals.h"
#include "Settings.h"
#include "InterfaceInfo.h"
#include "InterfaceStats.h"
#include "InterfaceSpeedMeter.h"

using namespace std;

//https://www.abc.se/~m6695/udp.html
//http://en.cppreference.com/w/cpp/links/libs
//http://jsoncpp.sourceforge.net/old.html

void* ServerThread::Thread( void )
{

    boost::asio::io_service io_service;


    uint32_t port;

    try
    {
        port = std::stoi( Settings::settings["stats server port"] );
    }
    catch ( ... )
    {
        port = 32000;
    }

    ServerThread::UDPServer s( io_service, port );

    io_service.run();

//	sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
//
//	bzero( &servaddr, sizeof( servaddr ) );
//	servaddr.sin_family = AF_INET;
//	servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
//	servaddr.sin_port = htons( port );
//	bind( sockfd, ( struct sockaddr * ) &servaddr, sizeof( servaddr ) );
//
//	string row;
//
//	while ( true )
//	{
//		len = sizeof( cliaddr );
//		memset( mesg, 0, 256 );
//		n = recvfrom( sockfd, mesg, 256, 0, ( struct sockaddr * ) &cliaddr, &len );
//
//		mesg[n] = 0;
//		string command( mesg );
//
//		if ( command.compare( "get stats" ) == 0 )
//		{
//			Json::Value root;
//			Json::StyledWriter writer;
//			uint32_t y;
//			uint32_t m;
//			uint32_t d;
//			uint32_t h;
//			Utils::get_time( &y, &m, &d, &h );
//
//			for ( auto const & mac_speedinfo : Globals::speed_stats )
//			{
//				const string& mac = mac_speedinfo.first;
//				const InterfaceSpeedMeter& ism = mac_speedinfo.second;
//
//				// speed is in bits/s
//				root[mac]["speed"]["down"] = Json::Value::UInt64( ism.get_rx_speed() );
//				root[mac]["speed"]["up"] = Json::Value::UInt64( ism.get_tx_speed() );
//
//				row.clear();
//				row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + "_" + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";
//
//				if ( Globals::all_stats[mac]["hourly"].find( row ) != Globals::all_stats[mac]["hourly"].end() )
//				{
//					root[mac]["hourly"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["hourly"][row].recieved() );
//					root[mac]["hourly"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["hourly"][row].transmited() );
//				}
//				else
//				{
//					root[mac]["hourly"]["down"] = Json::Value::UInt64( 0ULL );
//					root[mac]["hourly"]["up"] = Json::Value::UInt64( 0ULL );
//				}
//
//				row.clear();
//				row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );
//
//				if ( Globals::all_stats[mac]["daily"].find( row ) != Globals::all_stats[mac]["daily"].end() )
//				{
//					root[mac]["daily"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["daily"][row].recieved() );
//					root[mac]["daily"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["daily"][row].transmited() );
//				}
//				else
//				{
//					root[mac]["daily"]["down"] = Json::Value::UInt64( 0ULL );
//					root[mac]["daily"]["up"] = Json::Value::UInt64( 0ULL );
//				}
//
//				row.clear();
//				row += std::to_string( y ) + "-" + std::to_string( m );
//
//				if ( Globals::all_stats[mac]["monthly"].find( row ) != Globals::all_stats[mac]["monthly"].end() )
//				{
//					root[mac]["monthly"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["monthly"][row].recieved() );
//					root[mac]["monthly"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["monthly"][row].transmited() );
//				}
//				else
//				{
//					root[mac]["monthly"]["down"] = Json::Value::UInt64( 0ULL );
//					root[mac]["monthly"]["up"] = Json::Value::UInt64( 0ULL );
//				}
//
//				row.clear();
//				row += std::to_string( y );
//
//				if ( Globals::all_stats[mac]["yearly"].find( row ) != Globals::all_stats[mac]["yearly"].end() )
//				{
//					root[mac]["yearly"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["yearly"][row].recieved() );
//					root[mac]["yearly"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["yearly"][row].transmited() );
//				}
//				else
//				{
//					root[mac]["yearly"]["down"] = Json::Value::UInt64( 0ULL );
//					root[mac]["yearly"]["up"] = Json::Value::UInt64( 0ULL );
//				}
//			}
//
//			string response = writer.write( root );
//			sendto( sockfd, response.c_str(), response.size(), 0, ( struct sockaddr * ) &cliaddr, sizeof( cliaddr ) );
//		}
//		else if ( command.compare( "get interfaces" ) == 0 )
//		{
//			const map<string, InterfaceInfo>& interfaces = Utils::get_all_interfaces();
//
//			Json::Value root;
//			Json::StyledWriter writer;
//
//			for ( const auto& name_info : interfaces )
//			{
//				const string& name = name_info.first;
//				const InterfaceInfo& ii = name_info.second;
//
//				const string& mac = ii.get_mac();
//				const string& ip4 = ii.get_ip4();
//				const string& ip6 = ii.get_ip6();
//
//				root[name]["mac"] = mac;
//				root[name]["IP4"] = ip4;
//				root[name]["IP6"] = ip6;
//			}
//
//			string response = writer.write( root );
//			sendto( sockfd, response.c_str(), response.size(), 0, ( struct sockaddr * ) &cliaddr, sizeof( cliaddr ) );
//		}
//	}

    return 0;
}

/** @brief do_receive
  *
  * @todo: document this function
  */
void ServerThread::UDPServer::do_receive( void )
{
    memset( msg, 0, max_length * sizeof( char ) );
    m_socket.async_receive_from(
        boost::asio::buffer( msg, max_length ), m_sender_endpoint,
        [this]( boost::system::error_code ec, std::size_t bytes_recvd )
    {
        if ( !ec && bytes_recvd > 0 )
        {
            do_send( bytes_recvd );
        }
        else
        {
            do_receive();
        }
    } );
}


/** @brief do_send
  *
  * @todo: document this function
  */
void ServerThread::UDPServer::do_send( std::size_t length )
{
    string command( msg );
    string response;
    string row;

    if ( command.compare( "get stats" ) == 0 )
    {
        Json::Value root;
        Json::StyledWriter writer;
        uint32_t y;
        uint32_t m;
        uint32_t d;
        uint32_t h;
        Utils::get_time( &y, &m, &d, &h );

        for ( auto const & mac_speedinfo : Globals::speed_stats )
        {
            const string& mac = mac_speedinfo.first;
            const InterfaceSpeedMeter& ism = mac_speedinfo.second;

            // speed is in bits/s
            root[mac]["speed"]["down"] = Json::Value::UInt64( ism.get_rx_speed() );
            root[mac]["speed"]["up"] = Json::Value::UInt64( ism.get_tx_speed() );

            row.clear();
            row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d ) + "_" + std::to_string( h ) + ":00-" + std::to_string( h + 1 ) + ":00";

            if ( Globals::all_stats[mac]["hourly"].find( row ) != Globals::all_stats[mac]["hourly"].end() )
            {
                root[mac]["hourly"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["hourly"][row].recieved() );
                root[mac]["hourly"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["hourly"][row].transmited() );
            }
            else
            {
                root[mac]["hourly"]["down"] = Json::Value::UInt64( 0ULL );
                root[mac]["hourly"]["up"] = Json::Value::UInt64( 0ULL );
            }

            row.clear();
            row += std::to_string( y ) + "-" + std::to_string( m ) + "-" + std::to_string( d );

            if ( Globals::all_stats[mac]["daily"].find( row ) != Globals::all_stats[mac]["daily"].end() )
            {
                root[mac]["daily"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["daily"][row].recieved() );
                root[mac]["daily"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["daily"][row].transmited() );
            }
            else
            {
                root[mac]["daily"]["down"] = Json::Value::UInt64( 0ULL );
                root[mac]["daily"]["up"] = Json::Value::UInt64( 0ULL );
            }

            row.clear();
            row += std::to_string( y ) + "-" + std::to_string( m );

            if ( Globals::all_stats[mac]["monthly"].find( row ) != Globals::all_stats[mac]["monthly"].end() )
            {
                root[mac]["monthly"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["monthly"][row].recieved() );
                root[mac]["monthly"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["monthly"][row].transmited() );
            }
            else
            {
                root[mac]["monthly"]["down"] = Json::Value::UInt64( 0ULL );
                root[mac]["monthly"]["up"] = Json::Value::UInt64( 0ULL );
            }

            row.clear();
            row += std::to_string( y );

            if ( Globals::all_stats[mac]["yearly"].find( row ) != Globals::all_stats[mac]["yearly"].end() )
            {
                root[mac]["yearly"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["yearly"][row].recieved() );
                root[mac]["yearly"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["yearly"][row].transmited() );
            }
            else
            {
                root[mac]["yearly"]["down"] = Json::Value::UInt64( 0ULL );
                root[mac]["yearly"]["up"] = Json::Value::UInt64( 0ULL );
            }
        }

        response = writer.write( root );
    }
    else if ( command.compare( "get interfaces" ) == 0 )
    {
        const map<string, InterfaceInfo>& interfaces = Utils::get_all_interfaces();

        Json::Value root;
        Json::StyledWriter writer;

        for ( const auto& name_info : interfaces )
        {
            const string& name = name_info.first;
            const InterfaceInfo& ii = name_info.second;

            const string& mac = ii.get_mac();
            const string& ip4 = ii.get_ip4();
            const string& ip6 = ii.get_ip6();

            root[name]["mac"] = mac;
            root[name]["IP4"] = ip4;
            root[name]["IP6"] = ip6;
        }

        response = writer.write( root );
    }

    m_socket.async_send_to(
        boost::asio::buffer( response.c_str(), response.size() ), m_sender_endpoint,
        [this]( boost::system::error_code , std::size_t )
    {
        do_receive();
    } );
}





