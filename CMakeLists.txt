cmake_minimum_required(VERSION 3.26)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# for clangd lsp to work
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(web_server)
add_executable(run example.cpp)

# boost stuff, idk lol
# get rid of the warning
cmake_policy(SET CMP0167 NEW)
find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries(run PUBLIC Boost::system)
