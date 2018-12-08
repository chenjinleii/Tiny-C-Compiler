cmake_minimum_required(VERSION 3.12)
project(tcc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(Boost 1.68 REQUIRED COMPONENTS system)
find_package(LLVM REQUIRED CONFIG)
find_package(fmt)

add_definitions(${LLVM_DEFINITIONS})

file(GLOB cppsrc "*.h" "*.cpp")
add_executable(${PROJECT_NAME} ${cppsrc})

llvm_map_components_to_libnames(llvm_libs
                                support
                                core
                                irreader
                                ${LLVM_TARGETS_TO_BUILD})

target_link_libraries(${PROJECT_NAME}
                      ${Boost_LIBRARIES}
                      ${llvm_libs}
                      fmt::fmt
                      stdc++fs)