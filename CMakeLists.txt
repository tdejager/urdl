project(URDL C CXX)

cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

option(URDL_HEADER_ONLY "Use library as header only" OFF)
option(URDL_COMPILE_TESTS    "Compile the tests" OFF)
option(URDL_BUILD_EXAMPLES   "Built the examples" OFF)
option(URDL_USE_SSL          "Make use of SSL" OFF)

add_definitions(-DASIO_STANDALONE)

# Set the correct compiler flags
if(${CMAKE_CXX_COMPILER} STREQUAL "MSVC")
  if( MSVC_VERSION LESS 1700 )       # VC10-/VS2010- 
    message(FATAL_ERROR "The project requires C++11 features. " 
            "You need at least Visual Studio 11 (Microsoft Visual Studio 2012), " 
            "with Microsoft Visual C++ Compiler Nov 2012 CTP (v120_CTP_Nov2012).") 
  elseif( MSVC_VERSION EQUAL 1700 )  # VC11/VS2012
        message(STATUS "VC11:  using Microsoft Visual Studio 2012 " 
            "with Microsoft Visual C++ Compiler Nov 2012 CTP (v120_CTP_Nov2012) (MSVC_VERSION: ${MSVC_VERSION})" ) 
        set(CMAKE_GENERATOR_TOOLSET "v120_CTP_Nov2012" CACHE STRING "Platform Toolset" FORCE)             
  else() # VC12+, assuming C++11 supported. 
         message(STATUS "VC12:  using Microsoft Visual Studio 2013 " ) 
         #SET(CMAKE_GENERATOR_TOOLSET "Visual Studio 2013 Preview (v120)" CACHE STRING "Platform Toolset" FORCE)             
  endif()
  # Add multi-threaded compilation
  # not compatible with pre-compiled headers or minimum rebuild option
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP10")

elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    # Enable C++11 support on gcc
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
    # Set warnings are errors
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# FIND ASIO Libary
FIND_PATH(ASIO_INCLUDE_DIR
  NAMES
    asio.hpp
  PATHS
    /usr/include
    /usr/local/include
)


include_directories("include")
include_directories(${ASIO_INCLUDE_DIR})

if(URDL_HEADER_ONLY)
    add_definitions(-DURDL_HEADER_ONLY)
else()
    set(URDL_source "src/urdl.cpp")
    add_library(urdl_lib ${URDL_source})
endif()

if(USE_SSL)
  # Find OpenSSL Library
  find_package(OpenSSL REQUIRED)
  include_directories(
    ${OpenSSL_INCLUDE_DIR})
  target_link_libraries(urdl_lib ${OPENSSL_LIBRARIES})

else()
  add_definitions(-DURDL_DISABLE_SSL)
endif()


if(COMPILE_TESTS)
    set(test_include "test/http_server.hpp")
    add_executable(istream_test    ${test_include} "test/istream.cpp")
    add_executable(istreambuf_test ${test_include} "test/istreambuf.cpp")
    add_executable(option_set_test ${test_include} "test/option_set.cpp")
    add_executable(read_stream     ${test_include} "test/read_stream.cpp")
    add_executable(url             ${test_include} "test/url.cpp")
    if(NOT URDL_HEADER_ONLY)
        target_link_libraries(istream_test ${urdl_lib})
        target_link_libraries(istreambuf_test ${urdl_lib})
        target_link_libraries(option_set_test ${urdl_lib})
        target_link_libraries(read_stream ${urdl_lib})
        target_link_libraries(url ${urdl_lib})
    endif()
endif()

if(BUILD_EXAMPLES)
    add_executable(get1       "example/get1.cpp")
    add_executable(get2       "example/get2.cpp")
    add_executable(multiget1  "example/multiget1.cpp")
    add_executable(multiget2  "example/multiget2.cpp")
    if(NOT URDL_HEADER_ONLY)
        target_link_libraries(get1 urdl_lib)
        target_link_libraries(get2 urdl_lib)
        target_link_libraries(multiget1 urdl_lib)
        target_link_libraries(multiget2 urdl_lib)
    endif()
endif()




