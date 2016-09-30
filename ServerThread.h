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
            : m_socket( io_service, udp::endpoint( udp::v4(), port ) )
        {
            do_receive();
        }
    };


public:
    static void * Thread( void );
};

#endif // SERVERTHREAD_H
