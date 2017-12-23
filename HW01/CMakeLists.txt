cmake_minimum_required(VERSION 3.8)
project(hw01)

set(CMAKE_CXX_STANDARD 17)
file(GLOB hw01
        "*.h"
        "*.cpp"
        )
set(SOURCE_FILES ${hw01})
add_executable(hw01 ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message("ERROR: OpenGL not found")
endif(NOT OPENGL_FOUND)
find_package(glfw3)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(hw01 glfw ${OPENGL_gl_LIBRARY} glew AntTweakBar)