cmake_minimum_required(VERSION 3.5)
project(glhw02)

add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
)

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

set(ANTTWEAKBAR_SOURCE
        AntTweakBar-1.16/src/LoadOGL.cpp
        AntTweakBar-1.16/src/LoadOGLCore.cpp
        AntTweakBar-1.16/src/TwColors.cpp
        AntTweakBar-1.16/src/TwBar.cpp
        AntTweakBar-1.16/src/TwEventGLFW.c
        AntTweakBar-1.16/src/TwFonts.cpp
        AntTweakBar-1.16/src/TwMgr.cpp
        AntTweakBar-1.16/src/TwOpenGL.cpp
        AntTweakBar-1.16/src/TwOpenGLCore.cpp
        AntTweakBar-1.16/src/TwPrecomp.cpp
        )

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # If on Xcode, rename files to .mm
    file(COPY ${ANTTWEAKBAR_SOURCE} DESTINATION AntTweakBar-ObjectiveC/)
    file(RENAME ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/LoadOGL.cpp      ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/LoadOGL.mm     )
    file(RENAME ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/LoadOGLCore.cpp  ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/LoadOGLCore.mm )
    file(RENAME ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwColors.cpp     ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwColors.mm    )
    file(RENAME ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwBar.cpp        ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwBar.mm       )
    file(RENAME ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwEventGLFW.c    ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwEventGLFW.m  )
    file(RENAME ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwFonts.cpp      ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwFonts.mm     )
    file(RENAME ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwMgr.cpp        ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwMgr.mm       )
    file(RENAME ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwOpenGL.cpp     ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwOpenGL.mm    )
    file(RENAME ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwOpenGLCore.cpp ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwOpenGLCore.mm)
    file(RENAME ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwPrecomp.cpp    ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwPrecomp.mm   )

    set(ANTTWEAKBAR_SOURCE
            ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/LoadOGL.mm
            ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/LoadOGLCore.mm
            ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwColors.mm
            ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwBar.mm
            ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwEventGLFW.m
            ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwFonts.mm
            ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwMgr.mm
            ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwOpenGL.mm
            ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwOpenGLCore.mm
            ${CMAKE_BINARY_DIR}/external/AntTweakBar-ObjectiveC/TwPrecomp.mm
            )
    include_directories(
            ${CMAKE_SOURCE_DIR}/external/AntTweakBar-1.16/src/
    )
    add_definitions(
            -D_MACOSX
    )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(
            -D_UNIX
    )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(
            -D_WINDOWS
    )

endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


set(ANTTWEAKBAR_HEADERS
        AntTweakBar-1.16/src/AntPerfTimer.h
        AntTweakBar-1.16/src/LoadOGL.h
        AntTweakBar-1.16/src/LoadOGLCore.h
        AntTweakBar-1.16/src/MiniGLFW.h
        AntTweakBar-1.16/src/resource.h
        AntTweakBar-1.16/src/TwBar.h
        AntTweakBar-1.16/src/TwColors.h
        AntTweakBar-1.16/src/TwFonts.h
        AntTweakBar-1.16/src/TwGraph.h
        AntTweakBar-1.16/src/TwMgr.h
        AntTweakBar-1.16/src/TwOpenGL.h
        AntTweakBar-1.16/src/TwOpenGLCore.h
        AntTweakBar-1.16/src/TwPrecomp.h
        )

include_directories(
        AntTweakBar-1.16/include/
)

add_library( ANTTWEAKBAR_116_OGLCORE_GLFW STATIC
        ${ANTTWEAKBAR_SOURCE}
        ${ANTTWEAKBAR_HEADERS}
        )

target_link_libraries(ANTTWEAKBAR_116_OGLCORE_GLFW
        ${OPENGL_LIBRARY}
        ${EXTRA_LIBS}
        )


include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(glhw02 glfw ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} ${ASSIMP_LIBRARIES} ANTTWEAKBAR_116_OGLCORE_GLFW X11)