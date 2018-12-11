set(PROGRAM_NAME tcc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED CONFIG)
find_package(LLVM REQUIRED CONFIG)
find_package(fmt)
find_package(jsoncpp)

add_definitions(${LLVM_DEFINITIONS})

file(GLOB cppsrc "*.h" "*.cpp")
add_executable(${PROGRAM_NAME} ${cppsrc})

llvm_map_components_to_libnames(llvm_libs
                                support
                                core
                                irreader
                                ${LLVM_TARGETS_TO_BUILD})

target_link_libraries(${PROGRAM_NAME}
                      Qt5::Core
                      fmt::fmt
                      jsoncpp
                      ${llvm_libs}
                      stdc++fs)