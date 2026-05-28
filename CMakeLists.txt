cmake_minimum_required(VERSION 3.30)
project(GangodAudioPlayer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(GangodAudioPlayer
    src/main.cpp
    headers/gangod/reciver.h
    src/gangod/reciver.cpp
    include/json.hpp
    include/httplib.h
    headers/gangod/server.h
    headers/gangod/filemgmt.h
)
target_include_directories(GangodAudioPlayer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
)