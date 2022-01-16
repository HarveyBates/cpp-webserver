#include "server.hpp"


Server::Server(asio::io_context& io_context) : io_context_(io_context), 
	acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), kPort)){
	new_client();
	std::cout << "[Server]: Started." << std::endl;
}


Server::~Server(){
	io_context_.stop();
	std::cout << "[Sever]: Stopped." << std::endl;
}


void Server::new_client(){
	Connection::client newConnection = Connection::create(io_context_);
	acceptor_.async_accept(newConnection->socket(), 
			std::bind(&Server::handle_accept, this, newConnection,
				std::placeholders::_1));
}


void Server::handle_accept(Connection::client newConnection,
		const asio::error_code& err){
	if(asio::error::eof == err || 
			asio::error::connection_reset == err){
		std::cout << "[Server]: Error has occured. " << err << std::endl;
	}
	else{
		newConnection->do_read();
		new_client();
	}
}
