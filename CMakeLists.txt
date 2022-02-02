cmake_minimum_required(VERSION 3.14)

project(sqlite3 LANGUAGES C CXX)
add_library(sqlite3 STATIC)
add_subdirectory(src)
target_include_directories(sqlite3 INTERFACE src)

