cmake_minimum_required(VERSION 3.13)
project(tcc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED CONFIG)
find_package(Boost 1.69 REQUIRED COMPONENTS)
find_package(LLVM REQUIRED CONFIG)
find_package(fmt)
find_package(jsoncpp)

add_definitions(${LLVM_DEFINITIONS})

file(GLOB cppsrc "src/*.h" "src/*.cpp")
add_executable(${PROJECT_NAME} ${cppsrc})

llvm_map_components_to_libnames(llvm_libs
                                support
                                core
                                irreader
                                ${LLVM_TARGETS_TO_BUILD})

target_link_libraries(${PROJECT_NAME}
                      Qt5::Core
                      ${Boost_LIBRARIES}
                      ${llvm_libs}
                      fmt::fmt
                      jsoncpp
                      stdc++fs)