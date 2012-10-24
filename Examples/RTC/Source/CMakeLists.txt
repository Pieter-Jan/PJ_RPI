cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(RTC)

add_executable(RTC main.c)

target_link_libraries(RTC PJ_RPI)
