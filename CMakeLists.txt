cmake_minimum_required (VERSION 3.5)
project(XALMASCARPONE_LOGS)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(${CMAKE_BUILD_TYPE})
if(CMAKE_BUILD_TYPE MATCHES Debug)
  add_compile_definitions(DEBUG)
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/extlibs/rapidyaml ryml)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories("${PROJECT_SOURCE_DIR}/extlibs")
include_directories("${PROJECT_SOURCE_DIR}/extlibs/asio")

link_directories(${PROJECT_SOURCE_DIR}/extlibs/cryptopp)

set(
	SOURCES
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/LogMessage.cpp"
    "${PROJECT_SOURCE_DIR}/src/Server.cpp"
    "${PROJECT_SOURCE_DIR}/src/Client.cpp"
    "${PROJECT_SOURCE_DIR}/src/LogsProcessorClient.cpp"
    "${PROJECT_SOURCE_DIR}/src/IOServicePool.cpp"
)

if(WIN32) # Check if we are on Windows
  if( ${CMAKE_SYSTEM_VERSION} EQUAL 10)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
  endif()
elseif(UNIX)
  # Nothing special required
else()
  message(SEND_ERROR "You are on an unsupported platform! (Not Win32, Mac OS X or Unix)")
endif(WIN32)

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

target_link_libraries("xalmascarpone_logs" wsock32 ws2_32 ryml cryptopp)