cmake_minimum_required(VERSION 3.10)
project(tqueue C)

set(CMAKE_C_STANDARD 99)

add_executable(tqueue main.c waitfile.c waitfile.h mapmemory.c mapmemory.h tqueue_event.c tqueue_event.h)
target_link_libraries(tqueue pthread)