# hello bitches

this is a server with rest api written in c++, with help from the boost asio library

it uses 1 thread to respond to requests and another to asyncrhonously read/write to connections

this project is for educational purpouses, and im new to programming in general, so dont hit me : )

## api

pretty self explanarory, i think

theres also a few methods for the Request/Response to get/set data, here is an example

```cpp
std::uint16_t port = 6969;
web_server::Server my_server(port);

my_server.api.add_route("GET", "/", 
    [](const web_server::HTTP::Request&, web_server::HTTP::Response& Response) {
        Response.set_content("<h1>hello hi</h1>", "text/html");
    }
);

my_server.api.add_route("POST", "/hello", 
    [](const web_server::HTTP::Request& Request, web_server::HTTP::Response& Response) {
        Response.set_content(std::move(Request.body), "text");
    }
);

my_server.start();
```

and theres also a default 404 page for any other routes

## build & run

make sure you have a c++ compiler, cmake and boost installed, i think thats all?

to build and run the server,

```sh
mkdir ./build
cd ./build
cmake ..
make
./run
```

go to http://localhost:6969/ and enjoy
