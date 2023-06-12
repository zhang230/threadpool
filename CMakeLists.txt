cmake_minimum_required(VERSION 3.22.1)

project(TEST)

include_directories(
    ${CMAKE_SOURCE_DIR}/include/threadpool
)

file(GLOB THREAD_SOURCES ${CMAKE_SOURCE_DIR}/src/threadpool/threadpool.cpp)

add_executable(
    main 
    main.cpp
    ${THREAD_SOURCES}
)

