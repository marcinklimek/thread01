cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES ( /usr/local/include )
LINK_DIRECTORIES ( /usr/local/lib )

ADD_DEFINITIONS("-std=c++0x ")

FILE(GLOB src_files *.cpp)

get_filename_component (VAR ${CMAKE_CURRENT_LIST_DIR} NAME)  

project( ${VAR}  )


add_executable( ${VAR}  ${src_files} )

target_link_libraries( ${VAR} stdc++ pthread boost_thread boost_system zmq)
