cmake_minimum_required(VERSION 3.26)

project(http_server)
set (CMAKE_CXX_STANDARD 20)

# for clangd lsp to work
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(run main.cpp)

# TODO: compile as library and then make the example use the library
# but i wanna test faster so do this later
# add_library(lib SHARED web-server.hpp)

target_link_libraries(run LINK_PUBLIC ${Boost_LIBRARIES})
