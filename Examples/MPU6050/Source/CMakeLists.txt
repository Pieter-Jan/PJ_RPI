cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(IMU)

set(CURSES_INCLUDE_PATH /usr/include)
set(CURSES_LIBRARY /usr/lib/libcurses.so)
set(CURSES_NCURSES_LIBRARY /usr/lib/libncurses.so)

SET(CURSES_NEED_NCURSES TRUE)
FIND_PACKAGE(Curses)

add_library(MPU6050 MPU6050.c)
SET_TARGET_PROPERTIES(MPU6050 PROPERTIES LINKER_LANGUAGE C)

add_executable(IMU main.c)

target_link_libraries(IMU PJ_RPI)
target_link_libraries(IMU MPU6050)
target_link_libraries(IMU ncurses)
target_link_libraries(IMU -lm)		# link to math libraries
target_link_libraries(IMU -lrt)		# link to timer libraries
