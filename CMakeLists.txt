cmake_minimum_required(VERSION 3.10)
project(MyGame)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SDL2 and extensions using pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf>=2.0.0)

# Include directories for SDL2 and extensions
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

# Add a preprocessor definition for the project root directory
add_definitions(-DROOT_PATH="${CMAKE_SOURCE_DIR}/")

# Add the executable with the desired name 'game'
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(game ${SOURCES})

# Link SDL2 and extensions with your executable
target_link_libraries(game ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})
