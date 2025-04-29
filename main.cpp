#include "src/include.hpp"

int main() {
	try {
		web_server::Server my_server(6969);

		my_server.api.add_route("GET", "/", 
			[](const web_server::HTTP::Request&, web_server::HTTP::Response& Response) {
				Response.set_content("<h1>hello bitches</h1>", "text/html");
			}
		);

		my_server.api.add_route("POST", "/", 
			[](const web_server::HTTP::Request& Request, web_server::HTTP::Response& Response) {
				// Response.set_content(Request.body, Request.GetContentType());
				// Response.set_content("<h1>hello bitches</h1>", "text/html");
				Response.set_content(std::move(Request.body), "text");
			}
		);

		my_server.start();

		// TODO: detect ctrl + c somehow and shut off the server gracefully

	} catch (std::exception &e) {
		LOG(ERROR) << e.what();
	}

	return 0;
}
