#include "web-server.h"

int main() {
	try {
		boost::asio::io_context io_context;
		http_server::Server my_server(io_context, 6969);
		my_server.start();

		// TODO: detect ctrl + c somehow and shut off the server gracefully

	} catch (std::exception &e) {
		LOG(ERROR) << e.what();
	}

	return 0;
}
