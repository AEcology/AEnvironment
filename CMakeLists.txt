cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} [./])
project(AEnvironment)
set(CMAKE_C_FLAGS "-Wall -g -o")
INCLUDE_DIRECTORIES( . )


add_subdirectory(src)
add_subdirectory(include)

add_executable(AEnvironment ${SRCS} ${HEADERS})
