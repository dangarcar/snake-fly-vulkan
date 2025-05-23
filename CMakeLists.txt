cmake_minimum_required(VERSION 3.20)

project(FlyVulkan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(vulkan-engine)
add_subdirectory(Game)