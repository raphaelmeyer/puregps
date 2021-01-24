cmake_minimum_required(VERSION 3.19)

project(PureGPS)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(CTest)

add_subdirectory(source)
add_subdirectory(vendor)

# Settings
add_library(settings INTERFACE)
target_compile_options(settings INTERFACE -std=c++20 -Wall -Wextra -Werror)
