cmake_minimum_required(VERSION 3.16)
project(Rappy VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Find all source files
file(GLOB MAIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*/**.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
list(FILTER SRC_FILES EXCLUDE REGEX "build/.*")
list(FILTER HEADERS EXCLUDE REGEX "build/.*")

# Set the bin directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Define libraries to link with
set(LIBS -lpthread -lpigpio -lrt -lbluetooth -lfmt)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Make an object library.
add_library(common OBJECT ${SRC_FILES})
target_precompile_headers(common PUBLIC ${HEADERS})

# Add the executable
foreach(MAIN_FILE ${MAIN_FILES})
    get_filename_component(BIN_NAME ${MAIN_FILE} NAME_WE)
    add_executable(${BIN_NAME} ${MAIN_FILE} $<TARGET_OBJECTS:common>)
    target_link_libraries(${BIN_NAME} ${LIBS})
    add_custom_command(
        TARGET ${BIN_NAME} POST_BUILD
        COMMAND sudo chown root ${BIN_NAME}
        COMMAND sudo chmod u+s ${BIN_NAME}
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        VERBATIM)
endforeach()
