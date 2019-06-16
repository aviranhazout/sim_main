cmake_minimum_required(VERSION 3.14)
project(sim_main)

set(CMAKE_CXX_STANDARD 14)

add_executable(sim_main main.c core_api.c sim_mem.c)