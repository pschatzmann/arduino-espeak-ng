cmake_minimum_required(VERSION 3.16)

# set the project name
project(espeak-cmake)
set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

include(FetchContent)
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/arduino-espeak-ng )
endif()

# Build with Linux Arduino Audio Tools
FetchContent_Declare(arduino-audio-tools GIT_REPOSITORY "https://github.com/pschatzmann/arduino-audio-tools.git" GIT_TAG "main" )
FetchContent_GetProperties(arduino-audio-tools )
if(NOT arduino-audio-tools_POPULATED)
    FetchContent_Populate(arduino-audio-tools )
    add_subdirectory(${arduino-audio-tools_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/arduino-audio-tools )
endif()

# define libraries
add_executable(espeak-cmake espeak-cmake.cpp)
target_compile_definitions(espeak-cmake PUBLIC -DARDUINO -DIS_DESKTOP -DEXIT_ON_STOP)

target_link_libraries(espeak-cmake arduino-espeak-ng arduino_emulator portaudio_static arduino-audio-tools)