cmake_minimum_required(VERSION 3.6)
project(esp_ass)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    assa.c)

add_executable(esp_ass ${SOURCE_FILES})