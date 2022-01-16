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
		typedef std::shared_ptr<Connection> client;

		Connection(asio::io_context& io_context);
		~Connection();

		inline static client create(asio::io_context& io_context) {
			return client(new Connection(io_context));
		}

		inline asio::ip::tcp::socket& socket() {
			return socket_;
		}

		void do_read();

	private:

		static constexpr std::size_t kBufferSize = 4096;

		char buffer[kBufferSize];

		const std::string kWsMagicString = 
			"258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

		bool handshake = false;

		asio::ip::tcp::socket socket_;

		void do_write();

		void xor_decrypt(char* buffer);
};


#endif // CONNECTION_H_
