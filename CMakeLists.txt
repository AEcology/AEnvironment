cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} [./])
project(AEnvironment C CXX)
set(CMAKE_C_FLAGS "-Wall -g -o")
include_directories(src)
include_directories(include)

find_package(OpenGL REQUIRED)
find_package(GLUT   REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})


add_executable(AEnvironment
               ${OPENGL_LIBRARIES}
               ${GLUT_LIBRARY}
               include/glref.h
               include/glutil.h
               include/atoms.h
               include/automata.h
               include/brain.h
               include/evolution.h
               include/food.h
               include/linear.h
               include/neuralnet.h 
               include/environment.h
               src/environment.cpp
               src/main.cpp
               src/atoms.cpp
               src/automata.cpp
               src/brain.cpp
               src/evolution.cpp
               src/food.cpp
               src/glutil.cpp
               src/linear.cpp
               src/neuralnet.cpp )

target_link_libraries(AEnvironment ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
