cmake_minimum_required(VERSION 3.12)
project(epoll_using)

set(CMAKE_CXX_STANDARD 11)

add_executable(TestServer TestServer.cpp CSocket.h)
add_executable(TestClient TestClient.cpp CSocket.h)
add_executable(TestSocketEvent TestSocketEvent.cpp CSocketEvent.h CSocket.h)