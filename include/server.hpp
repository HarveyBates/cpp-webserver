#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>

#include <asio.hpp>
#include <asio/ts/buffer.h>
#include <asio/ts/internet.h>

#include "connection.hpp"


class Server {
	public:
		Server(asio::io_context& io_context);
		~Server();

	private:
		constexpr unsigned int kPort = 443;
		asio::io_context& io_context_;
		asio::ip::tcp::accpetor acceptor_;

		void start_accept();
		void handle_accept(Connection::pointer new_connection, 
				const asio::error_code& error);
}


#endif // SERVER_H_

