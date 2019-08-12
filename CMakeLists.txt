cmake_minimum_required(VERSION 3.2)
project(sg-mcu)

include(CMakeListsPrivate.txt)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeListsUser.txt)
include(CMakeListsUser.txt)
endif()

add_executable(${PROJECT_NAME} ${SRC_LIST} test)
