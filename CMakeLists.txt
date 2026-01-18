cmake_minimum_required(VERSION 3.10)
project(Henon_Viz CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Policy for OpenGL
cmake_policy(SET CMP0072 NEW)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Try to find GLFW3 with fallback to pkg-config
find_package(GLFW3 QUIET)
if(NOT GLFW3_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GLFW IMPORTED_TARGET glfw3)
        if(GLFW_FOUND)
            set(GLFW3_LIBRARIES PkgConfig::GLFW)
        endif()
    endif()
endif()

if(NOT GLFW3_FOUND AND NOT GLFW_FOUND)
    message(WARNING "GLFW3 not found via CMake or pkg-config, will try to link with -lglfw")
    set(GLFW3_LIBRARIES glfw)
else()
    if(GLFW3_FOUND)
        set(GLFW3_LIBRARIES GLFW::GLFW)
    endif()
endif()

# Find GLU library
find_library(GLU_LIBRARY NAMES GLU glu)
if(NOT GLU_LIBRARY)
    message(WARNING "GLU library not found, will try to link with -lGLU")
    set(GLU_LIBRARY GLU)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/viz)

# Add executables

# 1. PLY Exporter (no graphics libs needed)
add_executable(henon_ply_creator
    blender_viz/henon_ply_creator.cpp
)

# 2. Flow map field visualizer
add_executable(flow_map
    viz/src/flow_map.cpp
    viz/src/utils.cpp
)
target_link_libraries(flow_map
    PRIVATE ${GLFW3_LIBRARIES} OpenGL::OpenGL ${GLU_LIBRARY} GLUT::GLUT pthread dl m
)

# Set output directory for all targets
set_target_properties(henon_ply_creator flow_map
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Optional: enable optimizations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_options(-O2)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "OpenGL found: ${OPENGL_FOUND}")
message(STATUS "GLUT found: ${GLUT_FOUND}")
message(STATUS "GLFW3 libraries: ${GLFW3_LIBRARIES}")
