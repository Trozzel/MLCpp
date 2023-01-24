cmake_minimum_required(VERSION 3.17)
project (NeuralNetwork)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include
	../include include
	)

add_executable(NeuralNetwork
		src/main.cpp
		src/Neuron.cpp)

