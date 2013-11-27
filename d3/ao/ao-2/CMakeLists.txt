cmake_minimum_required(VERSION 2.8)

project(ao-2)

ADD_DEFINITIONS("-std=c++0x")

add_executable(ao-2 main.cpp)
target_link_libraries(ao-2  stdc++ boost_thread pthread)
