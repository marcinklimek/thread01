cmake_minimum_required(VERSION 2.8)

project(ao)

#ADD_DEFINITIONS("-std=c++0x")

add_executable(ao main.cpp)
target_link_libraries(ao stdc++  boost_thread pthread)
