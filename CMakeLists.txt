cmake_minimum_required(VERSION 3.22)
project(starwars_server)

set(CMAKE_CXX_STANDARD 23)

add_executable(starwars_server main.cpp Rotation.h InfoToSend.h boost_asio.h server.h)
