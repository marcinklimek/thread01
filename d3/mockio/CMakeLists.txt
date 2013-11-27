project(mockio)

cmake_minimum_required(VERSION 2.8)

ADD_DEFINITIONS("-std=c++11 -g")
ADD_DEFINITIONS("-std=gnu++11")

add_executable(mockio main.cpp)

target_link_libraries( mockio stdc++ pthread boost_thread boost_system boost_date_time )

install(TARGETS mockio RUNTIME DESTINATION bin )

