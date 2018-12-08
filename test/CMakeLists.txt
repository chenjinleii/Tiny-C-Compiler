set(TEST_NAME tcc_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(${PROJECT_SOURCE_DIR}/src)

find_package(Boost 1.68 REQUIRED COMPONENTS unit_test_framework)
find_package(LLVM REQUIRED CONFIG)

add_definitions(-DBOOST_TEST_DYN_LINK
                ${LLVM_DEFINITIONS})

file(GLOB cppsrc "${PROJECT_SOURCE_DIR}/src/*.h" "${PROJECT_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM cppsrc "${PROJECT_SOURCE_DIR}/src/main.cpp")
file(GLOB cpptest "*.h" "*.cpp")

add_executable(${TEST_NAME} ${cppsrc} ${cpptest})

llvm_map_components_to_libnames(llvm_libs
                                support
                                core
                                irreader
                                ${LLVM_TARGETS_TO_BUILD})

target_link_libraries(${TEST_NAME}
                      ${Boost_LIBRARIES}
                      ${llvm_libs}
                      stdc++fs)

add_test(NAME MyTest COMMAND ${TEST_NAME})