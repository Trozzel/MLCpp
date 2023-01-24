cmake_minimum_required(VERSION 3.22)
project(DataHandler)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
	DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ml_sandbox
        src/sandbox.cpp
		)
