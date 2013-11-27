# vim:set ts=2 sw=2 sts=2 et:

cmake_minimum_required(VERSION 2.6)
project(simpleamqpclient-examples)

FIND_PACKAGE(Boost 1.47.0 COMPONENTS chrono system thread REQUIRED)
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})

include_directories("./SimpleAmqpClient/")
include_directories("./")

ADD_EXECUTABLE(simple_connect simple_connect.cpp )
TARGET_LINK_LIBRARIES(simple_connect SimpleAmqpClient)



