cmake_minimum_required(VERSION 3.26)

project(http_server)
set (CMAKE_CXX_STANDARD 20)

# for clangd lsp to work
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(run main.cpp)

target_link_libraries(run LINK_PUBLIC ${Boost_LIBRARIES})
