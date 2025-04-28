#include "web-server.h"

int main() {
	try {
		boost::asio::io_context io_context;
		web_server::Server my_server(io_context, 6969);

		my_server.api.add_route("GET", "/bruh", 
			// [](const web_server::HTTP::HTTP_request& Request, web_server::HTTP::HTTP_response& Response) {
			[](const web_server::HTTP::HTTP_request&, web_server::HTTP::HTTP_response& Response) {
				Response.set_content("bruh hello bitch", "text");
			}
		);

		my_server.start();

		// TODO: detect ctrl + c somehow and shut off the server gracefully

	} catch (std::exception &e) {
		LOG(ERROR) << e.what();
	}

	return 0;
}
