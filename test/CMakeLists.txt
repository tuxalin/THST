cmake_minimum_required(VERSION 2.8)
project(test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

include_directories("${CMAKE_SOURCE_DIR}/../")

add_executable(thst_test main.cpp)

target_link_libraries(thst_test
	PRIVATE
		THST::THST
)
