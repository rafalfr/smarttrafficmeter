/*

ServerThread.cpp

Copyright (C) 2017 Rafał Frączek

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

/** @brief Thread
  *
  * The main UDP server function which is run as a thread
  *
  * @param void
  * @return void
  *
  */
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

    return nullptr;
}

/** @brief do_receive
  *
  * The private method that receives data
  *
  * @param void
  * @return void
  *
  */
void ServerThread::UDPServer::do_receive( void )
{
    memset( msg, 0, max_length * sizeof( char ) );
    m_socket.async_receive_from(
        boost::asio::buffer( msg, max_length ), m_sender_endpoint,
        [this]( boost::system::error_code ec, std::size_t bytes_recvd )
    {

    	if (Globals::terminate_program==true)
		{
			return;
		}


        if ( !ec && bytes_recvd > 0 )
        {
            do_send();
        }
        else
        {
            do_receive();
        }
    } );
}


/** @brief do_send
  *
  * The private method that sends data
  *
  * @param none
  * @return none
  *
  */
void ServerThread::UDPServer::do_send( void )
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
                root[mac]["hourly"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["hourly"][row].received() );
                root[mac]["hourly"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["hourly"][row].transmitted() );
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
                root[mac]["daily"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["daily"][row].received() );
                root[mac]["daily"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["daily"][row].transmitted() );
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
                root[mac]["monthly"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["monthly"][row].received() );
                root[mac]["monthly"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["monthly"][row].transmitted() );
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
                root[mac]["yearly"]["down"] = Json::Value::UInt64( Globals::all_stats[mac]["yearly"][row].received() );
                root[mac]["yearly"]["up"] = Json::Value::UInt64( Globals::all_stats[mac]["yearly"][row].transmitted() );
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
        [this]( boost::system::error_code, std::size_t )
    {
    	if (Globals::terminate_program==true)
		{
			return;
		}

        do_receive();
    } );
}
