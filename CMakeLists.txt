cmake_minimum_required (VERSION 2.6)
project (KaTaLyzer)

# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/src/backend/config.h.in"
  "${PROJECT_BINARY_DIR}/src/backend/config.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}/src/backend/")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

option(_PCAP "Use of the Pcap libraries" OFF)
option(CDP_P "Use CDP" OFF)
option(_SIP "Use SIP" OFF)
option(NETFLOW "Use NETFLOW" OFF)
option(_DEBUG_K "Enable debug" OFF)
option(CHCEM_POCTY "Enable debug dump" OFF)

find_package(MYSQL REQUIRED)
include_directories(${MYSQL_INCLUDE_DIRS})
find_package(Threads)
set(EXT_LIBS ${EXT_LIBS} ${MYSQL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

set(CMAKE_CXX_FLAGS "-g -Wall")

if(_PCAP)
find_package(PCAP REQUIRED)
include_directories(${PCAP_INCLUDE_DIRS})
set(EXT_LIBS ${EXT_LIBS} ${PCAP_LIBRARIES})
else()
add_subdirectory(src/backend/socket)
set(LIBS ${LIBS} ksocket)
endif()
add_subdirectory(src/backend)

