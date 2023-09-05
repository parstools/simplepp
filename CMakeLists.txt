cmake_minimum_required(VERSION 3.26)
project(pp)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-Werror=return-type")
endif()

add_executable(${PROJECT_NAME} main.cpp
        FileStack.cpp)
