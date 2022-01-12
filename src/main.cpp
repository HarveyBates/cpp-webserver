#include "main.h"


int main(){
	try{
		// io_context is an executor of scheduled tasks
		asio::io_context io_context;

		// Setup server using io_context
		Server server(io_context);

		io_context.run();
	}
	catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	
	return 0;
}
