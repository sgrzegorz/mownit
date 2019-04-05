cmake_minimum_required (VERSION 3.6)
project (myproject)

set (CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g  -O0 -Wall  -fPIC  -ldl") #-Wpedantic
set(SOURCE_FILES zad2.c)

add_executable(z ${SOURCE_FILES})

find_package(GSL REQUIRED)    # See below (2)
target_link_libraries(z GSL::gsl GSL::gslcblas)


#set_target_properties(pdynamic PROPERTIES COMPILE_FLAGS -O2) #-O -O1 -O2 -O3 -O0 -Os -Ofast -Og


