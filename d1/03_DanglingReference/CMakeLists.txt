cmake_minimum_required(VERSION 2.8)


#ADD_DEFINITIONS("-g -fmudflap -fmudflapth -funwind-tables -std=c++0x")
ADD_DEFINITIONS("-g  -funwind-tables -std=c++0x")

FILE(GLOB src_files *.cpp)

get_filename_component (VAR ${CMAKE_CURRENT_LIST_DIR} NAME)  

project( ${VAR}  )

add_executable( ${VAR}  ${src_files} )

target_link_libraries( ${VAR} stdc++ pthread boost_thread boost_system)
