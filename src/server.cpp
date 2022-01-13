#include "server.hpp"


Server::Server(asio::io_context& io_context) : io_context_(io_context), 
	acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), kPort)){
	start_accept();
}

Server::~Server(){
	io_context_.stop();
	std::cout << "[Sever]: Stopped server." << std::endl;
}


void Server::start_accept(){
	Connection::pointer newConnection = Connection::create(io_context_);
	acceptor_.async_accept(newConnection->socket(), 
			std::bind(&Server::handle_accept, this, newConnection,
				std::placeholders::_1));
}


void Server::handle_accept(Connection::pointer newConnection,
		const asio::error_code& err){
	if(!err){
		newConnection->start();
		start_accept();
	}
}
