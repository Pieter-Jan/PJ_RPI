cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Blink)

add_executable(Blink main.c)

target_link_libraries(Blink PJ_RPI)
