#ifndef SERVER_HTTP_HPP
#define	SERVER_HTTP_HPP

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/regex.hpp>

#include <unordered_map>
#include <thread>
#include <functional>
#include <iostream>
#include <sstream>
#include "Utils.h"

namespace SimpleWeb
{
template <class socket_type>
class ServerBase
{
public:
	class Response : public std::ostream
	{
		friend class ServerBase<socket_type>;
	private:
		boost::asio::yield_context& yield;

		boost::asio::streambuf streambuf;

		socket_type &socket;

		Response( socket_type &socket, boost::asio::yield_context& yield ):
			std::ostream( &streambuf ), yield( yield ), socket( socket ) {}

	public:
		size_t size()
		{
			return streambuf.size();
		}
		void flush()
		{
			boost::system::error_code ec;
			boost::asio::async_write( socket, streambuf, yield[ec] );

			if ( ec )
				throw std::runtime_error( ec.message() );
		}
	};

	class Content : public std::istream
	{
		friend class ServerBase<socket_type>;
	public:
		size_t size()
		{
			return streambuf.size();
		}
		std::string string()
		{
			std::stringstream ss;
			ss << rdbuf();
			return ss.str();
		}
	private:
		boost::asio::streambuf &streambuf;
		Content( boost::asio::streambuf &streambuf ): std::istream( &streambuf ), streambuf( streambuf ) {}
	};

	class Request
	{
		friend class ServerBase<socket_type>;
	public:
		std::string method, path, http_version;

		Content content;

		std::unordered_multimap<std::string, std::string> header;

		boost::smatch path_match;

		std::string remote_endpoint_address;
		unsigned short remote_endpoint_port;

	private:
		Request( boost::asio::io_service &io_service ): content( streambuf ), strand( io_service ) {}

		boost::asio::streambuf streambuf;

		boost::asio::strand strand;

		void read_remote_endpoint_data( socket_type& socket )
		{
			try
			{
				remote_endpoint_address = socket.lowest_layer().remote_endpoint().address().to_string();
				remote_endpoint_port = socket.lowest_layer().remote_endpoint().port();
			}
			catch ( const std::exception& e ) {}
		}
	};

	class Config
	{
		friend class ServerBase<socket_type>;
	private:
		Config( unsigned short port, size_t num_threads ): port( port ), num_threads( num_threads ), reuse_address( true ) {}
		unsigned short port;
		size_t num_threads;
	public:
		///IPv4 address in dotted decimal form or IPv6 address in hexadecimal notation.
		///If empty, the address will be any address.
		std::string address;
		///Set to false to avoid binding the socket to an address that is already in use.
		bool reuse_address;
	};
	///Set before calling start().
	Config config;

	std::unordered_map < std::string, std::unordered_map < std::string,
	    std::function<void( typename ServerBase<socket_type>::Response&, std::shared_ptr<typename ServerBase<socket_type>::Request> )> > >  resource;

	std::unordered_map < std::string,
	    std::function<void( typename ServerBase<socket_type>::Response&, std::shared_ptr<typename ServerBase<socket_type>::Request> )> > default_resource;

private:
	std::vector < std::pair < std::string, std::vector < std::pair < boost::regex,
	    std::function<void( typename ServerBase<socket_type>::Response&, std::shared_ptr<typename ServerBase<socket_type>::Request> )> > > > > opt_resource;

public:
	void start()
	{
		//Copy the resources to opt_resource for more efficient request processing
		opt_resource.clear();

		for ( auto& res : resource )
		{
			for ( auto& res_method : res.second )
			{
				auto it = opt_resource.end();

				for ( auto opt_it = opt_resource.begin(); opt_it != opt_resource.end(); opt_it++ )
				{
					if ( res_method.first == opt_it->first )
					{
						it = opt_it;
						break;
					}
				}

				if ( it == opt_resource.end() )
				{
					opt_resource.emplace_back();
					it = opt_resource.begin() + ( opt_resource.size() - 1 );
					it->first = res_method.first;
				}

				it->second.emplace_back( boost::regex( res.first ), res_method.second );
			}
		}

		if ( io_service.stopped() )
			io_service.reset();

		boost::asio::ip::tcp::endpoint endpoint;

		if ( config.address.size() > 0 )
			endpoint = boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( config.address ), config.port );
		else
			endpoint = boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), config.port );

		acceptor.open( endpoint.protocol() );
		acceptor.set_option( boost::asio::socket_base::reuse_address( config.reuse_address ) );
		acceptor.bind( endpoint );
		acceptor.listen();

		accept();

		//If num_threads>1, start m_io_service.run() in (num_threads-1) threads for thread-pooling
		threads.clear();

		for ( size_t c = 1; c < config.num_threads; c++ )
		{
			threads.emplace_back( [this]()
			{
				io_service.run();
			} );
		}

		//Main thread
		io_service.run();

		//Wait for the rest of the threads, if any, to finish as well
		for ( auto& t : threads )
		{
			t.join();
		}
	}

	void stop()
	{
		acceptor.close();
		io_service.stop();
	}

protected:
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::acceptor acceptor;
	std::vector<std::thread> threads;

	size_t timeout_request;
	size_t timeout_content;

	ServerBase( unsigned short port, size_t num_threads, size_t timeout_request, size_t timeout_send_or_receive ) :
		config( port, num_threads ), acceptor( io_service ),
		timeout_request( timeout_request ), timeout_content( timeout_send_or_receive ) {}

	virtual void accept() = 0;

	std::shared_ptr<boost::asio::deadline_timer> set_timeout_on_socket( std::shared_ptr<socket_type> socket, size_t seconds )
	{
		std::shared_ptr<boost::asio::deadline_timer> timer( new boost::asio::deadline_timer( io_service ) );
		timer->expires_from_now( boost::posix_time::seconds( seconds ) );
		timer->async_wait( [socket]( const boost::system::error_code & ec )
		{
			if ( !ec )
			{
				boost::system::error_code ec;
				socket->lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ec );
				socket->lowest_layer().close();
			}
		} );
		return timer;
	}

	std::shared_ptr<boost::asio::deadline_timer> set_timeout_on_socket( std::shared_ptr<socket_type> socket, std::shared_ptr<Request> request, size_t seconds )
	{
		std::shared_ptr<boost::asio::deadline_timer> timer( new boost::asio::deadline_timer( io_service ) );
		timer->expires_from_now( boost::posix_time::seconds( seconds ) );
		timer->async_wait( request->strand.wrap( [socket]( const boost::system::error_code & ec )
		{
			if ( !ec )
			{
				boost::system::error_code ec;
				socket->lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ec );
				socket->lowest_layer().close();
			}
		} ) );
		return timer;
	}

	void read_request_and_content( std::shared_ptr<socket_type> socket )
	{
		//Create new streambuf (Request::streambuf) for async_read_until()
		//shared_ptr is used to pass temporary objects to the asynchronous functions
		std::shared_ptr<Request> request( new Request( io_service ) );
		request->read_remote_endpoint_data( *socket );

		//Set timeout on the following boost::asio::async-read or write function
		std::shared_ptr<boost::asio::deadline_timer> timer;

		if ( timeout_request > 0 )
			timer = set_timeout_on_socket( socket, timeout_request );

		boost::asio::async_read_until( *socket, request->streambuf, "\r\n\r\n",
		                               [this, socket, request, timer]( const boost::system::error_code & ec, size_t bytes_transferred )
		{
			if ( timeout_request > 0 )
				timer->cancel();

			if ( !ec )
			{
				//request->streambuf.size() is not necessarily the same as bytes_transferred, from Boost-docs:
				//"After a successful async_read_until operation, the streambuf may contain additional data beyond the delimiter"
				//The chosen solution is to extract lines from the stream directly when parsing the header. What is left of the
				//streambuf (maybe some bytes of the content) is appended to in the async_read-function below (for retrieving content).
				size_t num_additional_bytes = request->streambuf.size() - bytes_transferred;

				parse_request( request, request->content );

				//If content, read that as well
				const auto it = request->header.find( "Content-Length" );

				if ( it != request->header.end() )
				{
					//Set timeout on the following boost::asio::async-read or write function
					std::shared_ptr<boost::asio::deadline_timer> timer;

					if ( timeout_content > 0 )
						timer = set_timeout_on_socket( socket, timeout_content );

					unsigned long long content_length;

					try
					{
						content_length = Utils::stoull( it->second );
					}
					catch ( const std::exception &e )
					{
						return;
					}

					if ( content_length > num_additional_bytes )
					{
						boost::asio::async_read( *socket, request->streambuf,
						                         boost::asio::transfer_exactly( content_length - num_additional_bytes ),
						                         [this, socket, request, timer]
						                         ( const boost::system::error_code & ec, size_t /*bytes_transferred*/ )
						{
							if ( timeout_content > 0 )
								timer->cancel();

							if ( !ec )
								find_resource( socket, request );
						} );
					}
					else
					{
						if ( timeout_content > 0 )
							timer->cancel();

						find_resource( socket, request );
					}
				}
				else
				{
					find_resource( socket, request );
				}
			}
		} );
	}

	void parse_request( std::shared_ptr<Request> request, std::istream& stream ) const
	{
		std::string line;
		getline( stream, line );
		size_t method_end;

		if ( ( method_end = line.find( ' ' ) ) != std::string::npos )
		{
			size_t path_end;

			if ( ( path_end = line.find( ' ', method_end + 1 ) ) != std::string::npos )
			{
				request->method = line.substr( 0, method_end );
				request->path = line.substr( method_end + 1, path_end - method_end - 1 );

				if ( ( path_end + 6 ) < line.size() )
					request->http_version = line.substr( path_end + 6, line.size() - ( path_end + 6 ) - 1 );
				else
					request->http_version = "1.0";

				getline( stream, line );
				size_t param_end;

				while ( ( param_end = line.find( ':' ) ) != std::string::npos )
				{
					size_t value_start = param_end + 1;

					if ( ( value_start ) < line.size() )
					{
						if ( line[value_start] == ' ' )
							value_start++;

						if ( value_start < line.size() )
							request->header.insert( std::make_pair( line.substr( 0, param_end ), line.substr( value_start, line.size() - value_start - 1 ) ) );
					}

					getline( stream, line );
				}
			}
		}
	}

	void find_resource( std::shared_ptr<socket_type> socket, std::shared_ptr<Request> request )
	{
		//Find path- and method-match, and call write_response
		for ( auto& res : opt_resource )
		{
			if ( request->method == res.first )
			{
				for ( auto& res_path : res.second )
				{
					boost::smatch sm_res;

					if ( boost::regex_match( request->path, sm_res, res_path.first ) )
					{
						request->path_match = std::move( sm_res );
						write_response( socket, request, res_path.second );
						return;
					}
				}
			}
		}

		auto it_method = default_resource.find( request->method );

		if ( it_method != default_resource.end() )
		{
			write_response( socket, request, it_method->second );
		}
	}

	void write_response( std::shared_ptr<socket_type> socket, std::shared_ptr<Request> request,
	                     std::function<void( typename ServerBase<socket_type>::Response&, std::shared_ptr<typename ServerBase<socket_type>::Request> )>& resource_function )
	{
		//Set timeout on the following boost::asio::async-read or write function
		std::shared_ptr<boost::asio::deadline_timer> timer;

		if ( timeout_content > 0 )
			timer = set_timeout_on_socket( socket, request, timeout_content );

		boost::asio::spawn( request->strand, [this, &resource_function, socket, request, timer]( boost::asio::yield_context yield )
		{
			Response response( *socket, yield );

			try
			{
				resource_function( response, request );
			}
			catch ( const std::exception& e )
			{
				return;
			}

			if ( response.size() > 0 )
			{
				try
				{
					response.flush();
				}
				catch ( const std::exception &e )
				{
					return;
				}
			}

			if ( timeout_content > 0 )
				timer->cancel();

			float http_version;

			try
			{
				http_version = Utils::stof( request->http_version );
			}
			catch ( const std::exception &e )
			{
				return;
			}

			if ( http_version > 1.05 )
				read_request_and_content( socket );
		} );
	}
};

template<class socket_type>
class Server : public ServerBase<socket_type> {};

typedef boost::asio::ip::tcp::socket HTTP;

template<>
class Server<HTTP> : public ServerBase<HTTP>
{
public:
	Server( unsigned short port, size_t num_threads = 1, size_t timeout_request = 5, size_t timeout_content = 300 ) :
		ServerBase<HTTP>::ServerBase( port, num_threads, timeout_request, timeout_content ) {}

private:
	void accept()
	{
		//Create new socket for this connection
		//Shared_ptr is used to pass temporary objects to the asynchronous functions
		std::shared_ptr<HTTP> socket( new HTTP( io_service ) );

		acceptor.async_accept( *socket, [this, socket]( const boost::system::error_code & ec )
		{
			//Immediately start accepting a new connection
			accept();

			if ( !ec )
			{
				boost::asio::ip::tcp::no_delay option( true );
				socket->set_option( option );

				read_request_and_content( socket );
			}
		} );
	}
};
}
#endif	/* SERVER_HTTP_HPP */
