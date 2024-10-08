cmake_minimum_required(VERSION 3.5.0)
project(sdltest)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(SourceFiles main.cpp)


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})


add_executable(${PROJECT_NAME} ${SourceFiles})


target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

# Copy the .obj file to the build directory
configure_file(VideoShip.obj ${CMAKE_BINARY_DIR} COPYONLY)

