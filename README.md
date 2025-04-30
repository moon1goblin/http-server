# hello bitches

this is a server with rest api written in c++, with help from the boost asio library

this project is for educational purpouses, and im new to programming in general, so dont hit me : )

## api

```cpp
web_server::Server my_server(6969);

my_server.api.add_route("GET", "/hello", 
    [](const web_server::HTTP::Request&, web_server::HTTP::Response& Response) {
        Response.set_content("<h1>hello bitches</h1>", "text/html");
    }
);

my_server.api.add_route("POST", "/", 
    [](const web_server::HTTP::Request& Request, web_server::HTTP::Response& Response) {
        Response.set_content(std::move(Request.body), "text");
    }
);

my_server.start();

```


## build & run

to build and run the server,

```sh
mkdir ./build
cd ./build
cmake ..
make
./run
```

go to http://localhost:6969/ and enjoy
