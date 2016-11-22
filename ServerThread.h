/*

ServerThread.h

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

/**
 * @file ServerThread.h
 * @brief Class definition for the ServerThread
 *
 * This file contains the definition of the ServerThread class.
 *
 * @author Rafał Frączek
 * @bug No known bugs
 *
 */

#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <boost/asio.hpp>

using boost::asio::ip::udp;

class ServerThread
{

private:

    class UDPServer
    {

    private:

        udp::socket m_socket;

        udp::endpoint m_sender_endpoint;

        static constexpr uint32_t max_length = 1024;

        char msg[max_length];

        void do_receive( void );

        void do_send( std::size_t length );

    public:

        UDPServer( boost::asio::io_service& io_service, short port )
            : m_socket( io_service, udp::endpoint( udp::v4(), port ) ), m_sender_endpoint()
        {
            do_receive();
        }
    };


public:

    static void * Thread( void );
};

#endif // SERVERTHREAD_H
