cmake_minimum_required (VERSION 3.13)

project(Walnut)

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(vendor)
add_subdirectory(Walnut)
add_subdirectory(WalnutApp)