cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 17)
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

set(SOURCES src/edge.cc src/star_chain_market.cc src/node.cc  src/linear_function.cc src/piecewise_linear_function.cc  src/linear_function_define_on_segment.cc)

################################
# GTest
################################
ADD_SUBDIRECTORY (googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_executable( runUnitTests ${SOURCES} ut/star_chain_market_test.cc ut/linear_function_define_on_segment.cc ut/linear_function_test.cc ut/helpers.h ut/piecewise_linear_function_test.cc ut/utils_test.cc  )
target_link_libraries(runUnitTests gtest gtest_main)
add_test( runUnitTests runUnitTests )


include_directories(include)
add_executable(start_chain_market main.cc ${SOURCES})
