cmake_minimum_required(VERSION 3.10)
project(my_minifb_project C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include FetchContent for downloading MiniFB
include(FetchContent)

# Declare MiniFB
FetchContent_Declare(
    minifb
    GIT_REPOSITORY https://github.com/emoon/minifb.git
    GIT_TAG master
)

# Make MiniFB available
FetchContent_MakeAvailable(minifb)

# Create executable
add_executable(my_app main.c)

# Link MiniFB
target_link_libraries(my_app PRIVATE minifb)

