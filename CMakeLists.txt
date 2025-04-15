cmake_minimum_required (VERSION 3.16)
project (smarttrafficmeter)

message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

include_directories(url)
include_directories(json)
include_directories(tclap)
include_directories(sqlite)
include_directories(serverhttp)
include_directories("${PROJECT_SOURCE_DIR}")

FIND_PACKAGE( Boost COMPONENTS program_options REQUIRED )

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/obj/release/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/release/)

file(GLOB SOURCES "*.c*")

## Compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-w -O3 -fexpensive-optimizations")        ## Optimize
    set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif()

add_executable(smarttrafficmeter ${SOURCES})
TARGET_LINK_LIBRARIES( smarttrafficmeter LINK_PUBLIC ${Boost_LIBRARIES} )
TARGET_LINK_LIBRARIES( smarttrafficmeter rt )
TARGET_LINK_LIBRARIES( smarttrafficmeter dl )
TARGET_LINK_LIBRARIES( smarttrafficmeter bfd )
TARGET_LINK_LIBRARIES( smarttrafficmeter curl )
TARGET_LINK_LIBRARIES( smarttrafficmeter pthread )
TARGET_LINK_LIBRARIES( smarttrafficmeter boost_system )
TARGET_LINK_LIBRARIES( smarttrafficmeter boost_regex )
TARGET_LINK_LIBRARIES( smarttrafficmeter boost_thread )
TARGET_LINK_LIBRARIES( smarttrafficmeter boost_filesystem )
TARGET_LINK_LIBRARIES( smarttrafficmeter boost_iostreams )
