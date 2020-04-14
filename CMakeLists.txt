cmake_minimum_required (VERSION 3.5)
project(XALMASCARPONE_LOGS)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories("${PROJECT_SOURCE_DIR}/extlibs")
include_directories("${PROJECT_SOURCE_DIR}/extlibs/asio")

set(
	SOURCES
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(USING_GCC TRUE)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(USING_CLANG TRUE)
endif()

if(USING_GCC OR USING_CLANG)
  add_compile_options(-std=c++17 -pedantic -Wall -Wl,-rpath)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(
    "xalmascarpone_logs"
    ${SOURCES}
)

target_link_libraries("xalmascarpone_logs" wsock32 ws2_32)