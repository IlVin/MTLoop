cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# DEBUG|RELEASE|RELWITHDEBINFO|MINSIZEREL
# SET (CMAKE_BUILD_TYPE DEBUG)
set (CMAKE_BUILD_TYPE DEBUG)

# Application name
set (PROJECT MTLoop)

# Project name
project (${PROJECT})

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g") # debug mode

# Directories
set (SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set (INC_DIR ${PROJECT_SOURCE_DIR}/libraries/MTLoop)
set (LIB_DIR ${PROJECT_SOURCE_DIR}/libraries/MTLoop)
set (TST_DIR ${PROJECT_SOURCE_DIR}/test)



######  JValue  ############
add_library (MTLoop_lib STATIC "${LIB_DIR}/MTLoop.cpp")
set (TARGET_LIBRARIES ${TARGET_LIBRARIES} MTLoop_lib)
include_directories (${INC_DIR})
###### /JValue  ############



######  BOOST  ############
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

find_package (Boost COMPONENTS unit_test_framework REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})
###### /BOOST  ############



###### TESTS  ############
enable_testing ()
###### /TESTS  ############



######  EXECUTABLE  ############
add_executable ("${PROJECT}.exe" "${SRC_DIR}/main.cpp")
add_executable ("${PROJECT}_ut.exe" "${SRC_DIR}/MTLoop_ut.cpp")
###### /EXECUTABLE  ############



######  LINKING LIBRARY  ############
foreach (LIBRARY ${TARGET_LIBRARIES})
    target_link_libraries(${LIBRARY})
    #    target_link_libraries(${PROJECT} ${LIBRARY})
endforeach ()
###### /LINKING LIBRARY  ############





