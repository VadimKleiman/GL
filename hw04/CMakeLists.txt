cmake_minimum_required(VERSION 3.8)
project(hw04)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_FLAGS "-framework OpenCL")
file(GLOB hw04
        "*.h"
        "*.cpp"
        )
set(SOURCE_FILES ${hw04})
add_executable(hw04 ${SOURCE_FILES})

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

find_package(Qt5Widgets)
find_package(Qt5OpenGL)
set(CMAKE_AUTOMOC ON)
qt5_use_modules(hw04 Widgets OpenGL)

include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(hw04 glfw ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} ${QT_LIBRARIES})