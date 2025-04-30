#include "src/include.hpp"

int main() {
	try {
		std::uint16_t port = 6969;
		web_server::Server my_server(port);

		my_server.api.add_route("GET", "/", 
			[](const web_server::HTTP::Request&, web_server::HTTP::Response& Response) {
				Response.set_content("<h1>hello hi</h1>", "text/html");
			}
		);

		my_server.api.add_route("POST", "/hello", 
			[](const web_server::HTTP::Request& Request, web_server::HTTP::Response& Response) {
				Response.set_content(std::move(Request.body), Request.GetContentType());
			}
		);

		my_server.start();

		// TODO: detect ctrl + c somehow and shut off the server gracefully

	} catch (std::exception &e) {
		LOG(ERROR) << e.what();
	}

	return 0;
}
