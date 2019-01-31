cmake_minimum_required (VERSION 2.6)
project (ransel)

set (RANSEL_VERSION_MAJOR 1)
set (RANSEL_VERSION_MINOR 0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set (LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set (SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/ransel.cpp)

add_executable(ransel ${SOURCE_FILES})
target_include_directories(ransel PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(ransel PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(ransel PUBLIC stdc++fs)
