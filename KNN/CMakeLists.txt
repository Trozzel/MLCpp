cmake_minimum_required(VERSION 3.22)
project(KNN)

set(CMAKE_CXX_STANDARD 20)

include_directories(include
        ../include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

add_executable(knn
        src/main.cpp
        )

add_executable(knn_sandbox
        src/knn_sandbox.cpp
        )
target_link_libraries(knn_sandbox Threads::Threads)