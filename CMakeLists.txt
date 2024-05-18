cmake_minimum_required(VERSION 3.0)
project(Rappy VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Define the relevant directories
set(DEPDIR "${CMAKE_CURRENT_SOURCE_DIR}/dep")
set(OBJDIR "${CMAKE_CURRENT_BINARY_DIR}/obj")
set(BINDIR "${CMAKE_CURRENT_BINARY_DIR}/bin")

# Create the directories
file(MAKE_DIRECTORY ${DEPDIR})
file(MAKE_DIRECTORY ${OBJDIR})
file(MAKE_DIRECTORY ${BINDIR})

# Find all source files
file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Define libraries to link with
set(LIBS -lpthread -lpigpio -lrt -lbluetooth -lfmt)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

# Add the executable
foreach(SRC_FILE ${SRC_FILES})
    get_filename_component(BIN_NAME ${SRC_FILE} NAME_WE)
    add_executable(${BIN_NAME} ${SRC_FILE})
    target_link_libraries(${BIN_NAME} ${LIBS})
    set_target_properties(${BIN_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINDIR})
endforeach()

# Custom target for clean command
add_custom_target(clean
   COMMAND ${CMAKE_COMMAND} -P clean.cmake
)

# Create clean-all.cmake script
file(WRITE clean.cmake "
file(REMOVE_RECURSE ${OBJDIR}/*)
file(REMOVE_RECURSE ${DEPDIR}/*)
file(REMOVE_RECURSE ${BINDIR}/*)
")
