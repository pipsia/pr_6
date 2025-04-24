cmake_minimum_required(VERSION 3.10)
project(prac6_sysprog C)

set(CMAKE_C_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(task1 src/task1.c)
target_link_libraries(task1 Threads::Threads)

add_executable(task2 src/task2.c)
target_link_libraries(task2 Threads::Threads)
