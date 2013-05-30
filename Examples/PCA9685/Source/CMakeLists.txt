cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(PWM)

set(CURSES_INCLUDE_PATH /usr/include)
set(CURSES_LIBRARY /usr/lib/libcurses.so)
set(CURSES_NCURSES_LIBRARY /usr/lib/libncurses.so)

add_library(PCA9685 PCA9685.c)
SET_TARGET_PROPERTIES(PCA9685 PROPERTIES LINKER_LANGUAGE C)

add_executable(PWM main.c)

target_link_libraries(PWM PJ_RPI)
target_link_libraries(PWM PCA9685)
target_link_libraries(PWM -lm)		# link to math libraries
target_link_libraries(PWM -lrt)	# link to timer libraries
