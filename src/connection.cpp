#include "connection.hpp"


Connection::Connection(asio::io_context& io_context) : 
	socket_(io_context){

}


Connection::~Connection(){
	std::error_code ec;
	socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
	socket_.close();
}


void Connection::xor_decrypt(char* buffer){
	char outBuffer[kBufferSize];

	unsigned int packetLength = 0;
	char mask[4];

	std::fill_n(outBuffer, kBufferSize, 0); // This is gross

	if((unsigned char)buffer[0] == 0x81){
		packetLength = buffer[1] & 0x7f;

		if(packetLength == 126){
			unsigned int payloadLength = (buffer[2] << 8 & 0xFF) + 
				(buffer[3] & 0x00);

			mask[0] = buffer[4];
			mask[1] = buffer[5];
			mask[2] = buffer[6];
			mask[3] = buffer[7];

			int offset = 8;
			for(int i = 0; i < payloadLength; i++){
				int pos = offset + 8;
				outBuffer[i] = buffer[pos] ^ mask[i % 4];
			}
		}
		else{
			mask[0] = buffer[2];
			mask[1] = buffer[3];
			mask[2] = buffer[4];
			mask[3] = buffer[5];

			int offset = 6;
			for(int i = 0; i < packetLength; i++){
				int pos = offset + i;
				outBuffer[i] = buffer[pos] ^ mask[i % 4];
			}
		}
	}

	std::cout << "> " << outBuffer << std::endl;
}


void Connection::start(){
	// Equivilent to do_read()
	auto self(shared_from_this());

	std::fill_n(buffer, kBufferSize, 0); // This is gross

	socket_.async_read_some(asio::buffer(buffer, kBufferSize), 
			[this, self](std::error_code err, size_t length){
		
		if(asio::error::eof == err || 
				asio::error::connection_reset == err){
			std::cout << 
				"[Connection]: Client has disconnected." 
			<< std::endl;
			handshake = false;
		}

		else{
			if(handshake){
				xor_decrypt(buffer);
			}
			else{
				std::cout << 
					"[Connection]: Handshake."
				<< std::endl;
				handshake = true;
			}

			do_write();
		}
	});
}

void Connection::do_write(){
	auto self(shared_from_this());

	std::string tmp(buffer);
	std::string response;

	if(tmp.find("Sec-WebSocket-Key: ") != std::string::npos){
		tmp.erase(0, tmp.find("Sec-WebSocket-Key: ") + 
				strlen("Sec-WebSocket-Key: "));
		auto key = tmp.substr(0, tmp.find("\r\n"));

		auto sha1Key = Crypto::sha1(key + kWsMagicString);

		sha1Key = Crypto::Base64::encode(sha1Key);

		response = 
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: websocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: " + sha1Key + "\r\n\r\n";
	}
	else{
		response = Crypto::Base64::decode(buffer);
	}

	asio::async_write(socket_, asio::buffer(response.c_str(), 
				response.size()), [this, self](std::error_code err,
				size_t length){
		if(!err){
			start();
		}
		// TODO What if there is an error
	});
}





