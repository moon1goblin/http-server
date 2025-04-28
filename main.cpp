#include "src/server.hpp"
#include "web-server.h"

int main() {
	try {
		boost::asio::io_context io_context;
		web_server::Server my_server(io_context, 6969);

		my_server.api.add_route("GET", "/", 
			[](const web_server::HTTP::Request&, web_server::HTTP::Response& Response) {
				Response.set_content("<h1>hello bitches</h1>", "text/html");
			}
		);

		my_server.api.add_route("GET", "/example", 
			[](const web_server::HTTP::Request& Request, web_server::HTTP::Response& Response) {
				Response.set_content(Request.directory, "text");
			}
		);

		my_server.start();

		io_context.run();

		// TODO: detect ctrl + c somehow and shut off the server gracefully

	} catch (std::exception &e) {
		LOG(ERROR) << e.what();
	}

	return 0;
}
