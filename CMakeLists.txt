cmake_minimum_required(VERSION 3.10)

project(UECustomNet)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/ExampleProject/ThirdParty/UECustomNet")

set(SLIKENET_DIR ${CMAKE_SOURCE_DIR}/Depends/slikenet-0.1.3)

add_subdirectory(${SLIKENET_DIR})

add_subdirectory(Source/Core)
add_subdirectory(Source/Server)
