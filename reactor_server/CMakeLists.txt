cmake_minimum_required(VERSION 3.10)
project(ReactorServer)

AUX_SOURCE_DIRECTORY(./ SRC_LIST)
SET(EXECUTABLE_OUTPUT_PATH ./)
ADD_DEFINITIONS(-g -W -Wall -Wno-deprecated
        -DLINUX -D_REENTRANT -D_FILE_OFFSET_BITS=64
        -DAC_HAS_INFO -DAC_HAS_WARNING -DAC_HAS_ERROR
        -DAC_HAS_CRITICAL -DTIXML_USE_STL
        -DHAVE_CXX_STDHEADERS ${CMAKE_CXX_FLAGS}
        -std=c++11)
INCLUDE_DIRECTORIES(./)
LINK_DIRECTORIES(./)
SET(main.cpp ReactorServer.cpp Log.cpp)
ADD_EXECUTABLE(ReactorServer ${SRC_LIST})
TARGET_LINK_LIBRARIES(ReactorServer pthread)