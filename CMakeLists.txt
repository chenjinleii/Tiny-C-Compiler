cmake_minimum_required(VERSION 3.12)
project(tcc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core REQUIRED CONFIG)
find_package(Qt5Sql REQUIRED CONFIG)
find_package(Qt5Widgets REQUIRED CONFIG)
find_package(Boost 1.68 REQUIRED COMPONENTS system)
find_package(LLVM REQUIRED CONFIG)
find_package(fmt)
find_package(jsoncpp)

add_definitions(-DBOOST_ASIO_NO_DEPRECATED
                ${LLVM_DEFINITIONS})

file(GLOB cppsrc "src/*.h" "src/*.cpp")
add_executable(${PROJECT_NAME} ${cppsrc})

llvm_map_components_to_libnames(llvm_libs
                                support
                                core
                                irreader
                                ${LLVM_TARGETS_TO_BUILD})

target_link_libraries(${PROJECT_NAME}
                      Qt5::Core
                      Qt5::Sql
                      Qt5::Widgets
                      jsoncpp
                      ${Boost_LIBRARIES}
                      ${llvm_libs}
                      fmt::fmt
                      stdc++fs
                      pthread)