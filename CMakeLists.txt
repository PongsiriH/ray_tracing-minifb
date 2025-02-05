cmake_minimum_required(VERSION 3.10)
project(my_minifb_project C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include FetchContent for downloading MiniFB
include(FetchContent)

# Set MiniFB options before fetching
set(USE_WAYLAND_API OFF CACHE BOOL "Use Wayland API")
set(USE_OPENGL_API OFF CACHE BOOL "Use OpenGL API")
set(MINIFB_BUILD_EXAMPLES OFF CACHE BOOL "Build MiniFB example programs")
set(OPTIONS_USE_METAL_API OFF CACHE BOOL "Use Metal API")
set(MINIFB_BUILD_TESTS OFF CACHE BOOL "Build MiniFB tests")

# Declare MiniFB
FetchContent_Declare(
    minifb
    GIT_REPOSITORY https://github.com/emoon/minifb.git
    GIT_TAG master
)

# Make MiniFB available
FetchContent_MakeAvailable(minifb)

# Collect all source files
file(GLOB_RECURSE SOURCES 
    "src/*.c"
)

# Create executable
add_executable(my_app 
  ${SOURCES}
  src/somehelper.c
)

# Add include directory for header files
target_include_directories(my_app PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link MiniFB
target_link_libraries(my_app PRIVATE minifb)

target_link_libraries(my_app PRIVATE m)

