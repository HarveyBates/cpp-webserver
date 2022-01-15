#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include "connection.hpp"

const unsigned int kPort = 443;

class Server {
	public:
		Server(asio::io_context& io_context);
		~Server();

	private:
		asio::io_context& io_context_;
		asio::ip::tcp::acceptor acceptor_;

		void start_accept();
		void handle_accept(Connection::client newConnection, 
				const asio::error_code& error);
};


#endif // SERVER_H_

