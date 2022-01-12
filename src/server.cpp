#include "server.h"


Server::Server(asio::io_context& io_context) : io_context_(io_context),
	acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(),
				port)){
		start_accept();
}


Server::~Server(){
	io_context_.stop();
	std::cout << "[Sever]: Stopped server." << std::endl;
}


void Server::start_accept(){
	pointer newConnection = create(io_context_);
	acceptor_.async_accept(newConnection->socket(), 
			std::bind(&handle_accept, this, new_connection,
				std::placeholders::_1));
}


void Server::handle_accept(pointer newConnection,
		const asio::error_code& err){
	if(!err){
		newConnection->start();
		start_accept();
	}
}
