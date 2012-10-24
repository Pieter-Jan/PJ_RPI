cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project (PJ_RPI)

add_library(PJ_RPI PJ_RPI_GPIO.c)

install (TARGETS PJ_RPI DESTINATION lib)

install (FILES PJ_RPI_GPIO.h DESTINATION include)

