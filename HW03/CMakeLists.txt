cmake_minimum_required(VERSION 3.7)
project(glhw02)

set(CMAKE_CXX_STANDARD 14)
file(GLOB glhw02
        "*.h"
        "*.cpp"
        )
set(SOURCE_FILES ${glhw02})
add_executable(glhw02 ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message("ERROR: OpenGL not found")
endif(NOT OPENGL_FOUND)
find_package(glfw3)
find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()
find_package(assimp REQUIRED)
if (GLEW_FOUND)
    include_directories(${ASSIMP_INCLUDE_DIRS})
    link_libraries(${ASSIMP_LIBRARIES})
endif()

include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(glhw02 glfw ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} ${ASSIMP_LIBRARIES})