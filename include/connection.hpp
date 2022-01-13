#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <iostream>
#include <string.h>

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <asio/ssl.hpp>

#include "crypto.hpp"


class Connection : public std::enable_shared_from_this<Connection>{
	public:
		typedef std::shared_ptr<Connection> pointer;

		static pointer create(asio::io_context& io_context);
		asio::ip::tcp::socket& socket();
		void start();

	private:
		char buffer[64000];
		bool handshake = false;
		asio::ip::tcp::socket socket_;

		inline static const std::string kWsMagicString = 
			"258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		inline static const std::size_t kBufferLength = 4096;

		Connection(asio::io_context& io_context);
		~Connection();

		void handle_write(const asio::error_code& error, 
				size_t bytesTransfer);
		void do_read();
		void do_write();
		void xor_decrypt(char* buffer);
};


#endif // CONNECTION_H_
