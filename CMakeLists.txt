cmake_minimum_required(VERSION 3.16)

# set the project name
project(arduino-espeak-ng)

# lots of warnings and all warnings as errors
# add_compile_options(-Wall -Wextra )

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
include(FetchContent)

# Build with Linux Arduino Emulator
FetchContent_Declare(arduino_emulator GIT_REPOSITORY "https://github.com/pschatzmann/Arduino-Emulator.git" GIT_TAG main )
FetchContent_GetProperties(arduino_emulator)
if(NOT arduino_emulator_POPULATED)
    FetchContent_Populate(arduino_emulator)
    add_subdirectory(${arduino_emulator_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/arduino_emulator)
endif()

FetchContent_Declare(arduino-posix-fs GIT_REPOSITORY "https://github.com/pschatzmann/arduino-posix-fs.git" GIT_TAG main )
FetchContent_GetProperties(arduino-posix-fs)
if(NOT arduino-posix-fs_POPULATED)
    FetchContent_Populate(arduino-posix-fs)
    add_subdirectory(${arduino-posix-fs_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/arduino-posix-fs)
endif()


# define source files
file(GLOB_RECURSE SRC_LIST_CPP CONFIGURE_DEPENDS  "${PROJECT_SOURCE_DIR}/src/*.cpp" )
file(GLOB_RECURSE SRC_LIST_C CONFIGURE_DEPENDS  "${PROJECT_SOURCE_DIR}/src/*.c" )

# define libraries
add_library (arduino-espeak-ng STATIC ${SRC_LIST_CPP} ${SRC_LIST_C})

target_compile_definitions(arduino-espeak-ng PUBLIC -DIS_DESKTOP )
target_compile_options(arduino-espeak-ng PUBLIC -Wframe-larger-than=1000)
target_compile_definitions(arduino-posix-fs PUBLIC -DIS_DESKTOP )


# define location for header files
target_include_directories(arduino-espeak-ng PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )

target_link_libraries(arduino-espeak-ng arduino-posix-fs arduino_emulator)