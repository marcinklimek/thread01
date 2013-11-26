cmake_minimum_required(VERSION 2.8)

#ADD_DEFINITIONS("-std=c++0x")

set( CMAKE_CXX_FLAGS "-g -O0 " )

FILE(GLOB src_files *.cpp)

get_filename_component (VAR ${CMAKE_CURRENT_LIST_DIR} NAME)  

project( ${VAR}  )

add_executable( ${VAR}  ${src_files} )

target_link_libraries( ${VAR} stdc++ pthread boost_thread boost_system)
