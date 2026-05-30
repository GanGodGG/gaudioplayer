cmake_minimum_required(VERSION 3.30)
project(GangodAudioPlayer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SDL3")

find_package(SDL3 REQUIRED)
add_executable(GangodAudioPlayer
    src/main.cpp
    headers/gangod/reciver.h
    src/gangod/reciver.cpp
    include/json.hpp
    include/httplib.h
    headers/gangod/server.h
    src/gangod/server.cpp
    headers/gangod/filemgmt.h
    src/gangod/graphics.cpp
    headers/gangod/graphics.h
    include/stb_image.h
)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SDL3/bin/SDL3.lib")
    target_link_libraries(GangodAudioPlayer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SDL3/bin/SDL3.lib")
else()
    target_link_libraries(GangodAudioPlayer PRIVATE SDL3::SDL3)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SDL3/bin/SDL3_image.lib")
    target_link_libraries(GangodAudioPlayer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SDL3/bin/SDL3_image.lib")
else()
    target_link_libraries(GangodAudioPlayer PRIVATE SDL3_image::SDL3_image)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SDL3/bin/SDL3_ttf.lib")
    target_link_libraries(GangodAudioPlayer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SDL3/bin/SDL3_ttf.lib")
else()
    target_link_libraries(GangodAudioPlayer PRIVATE SDL3_ttf::SDL3_ttf)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Fmod/lib/fmod_vc.lib")
    target_link_libraries(GangodAudioPlayer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Fmod/lib/fmod_vc.lib")
else()
    target_link_libraries(GangodAudioPlayer PRIVATE FMod::FMod)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Fmod/lib/fmodL_vc.lib")
    target_link_libraries(GangodAudioPlayer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Fmod/lib/fmodL_vc.lib")
else()
    target_link_libraries(GangodAudioPlayer PRIVATE FMod::FMod)
endif()
target_include_directories(GangodAudioPlayer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

# Prevent SDL from replacing `main` with its own entry-point on Windows
target_compile_definitions(GangodAudioPlayer PRIVATE SDL_MAIN_HANDLED)