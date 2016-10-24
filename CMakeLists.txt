#---------------------------------------------------------------------
#   Author: Kuumies <kuumies@gmail.com>
#   Desc:   Main make file of the QOpenGLWidget multi-thread example
#---------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0.0)
project(qopenglwidget-multithread-example)

#---------------------------------------------------------------------
# Setup compiler and linker to support C++ 11 etc.

if(MSVC)
    IF(CMAKE_CL_64)
        SET_PROPERTY(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ON)
    ENDIF(CMAKE_CL_64)

    SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} /D_AFXDLL /EHsc /W 3 /D_CRT_SECURE_NO_WARNINGS /Zo /FS")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_AFXDLL /EHsc /W 3 /D_CRT_SECURE_NO_WARNINGS /Zo /FS")
else(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ggdb -g3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(MSVC)

#---------------------------------------------------------------------
# Find packages and libraries. For windows use the GLEW from the
# external directory.

if(MSVC)
    set(GLEW_DIR ${CMAKE_CURRENT_LIST_DIR}/external/glew)
    set(GLEW_INCLUDE_DIR ${GLEW_DIR}/include/)
    set(GLEW_LIBRARIES ${GLEW_DIR}/lib/glew32.lib)
    include_directories(${GLEW_INCLUDE_DIR})
    install(FILES ${GLEW_DIR}/bin/glew32.dll DESTINATION bin)
endif(MSVC)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(OpenGL REQUIRED)

set(GLM_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/external/glm)
include_directories(${GLM_INCLUDE_DIR})

#---------------------------------------------------------------------
# Set sources.

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_wrap_cpp(MOC_SOURCE
    src/opengl_renderer_object.h
    src/opengl_widget.h
)

qt5_add_resources(RCC_RESOURCES
    resource/qopenglwidget-multithread-example.qrc
)

set(SOURCE
    src/main.cpp
    src/opengl.h
    src/opengl_renderer_object.cpp
    src/opengl_widget.cpp
)

#---------------------------------------------------------------------
# Add executable and link the needed libraries (Qt, OpenGL (+GLEW
# on windows))

if (APPLE)
    # Set the @rpath to Qt libs path (this affects only the installed
    # application binary)
    set(CMAKE_INSTALL_RPATH "${CMAKE_PREFIX_PATH}/lib")
endif(APPLE)

add_executable(${PROJECT_NAME} ${SOURCE} ${RCC_RESOURCES} ${MOC_SOURCE})
target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Qt5::OpenGL
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
)

#---------------------------------------------------------------------
# Install binary and runtime to 'bin' folder

include(InstallRequiredSystemLibraries)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

if (MSVC)
    get_target_property(Qt5_CoreLocation Qt5::Core LOCATION)
    install(FILES ${Qt5_CoreLocation} DESTINATION bin)
    get_target_property(Qt5_GuiLocation Qt5::Gui LOCATION)
    install(FILES ${Qt5_GuiLocation} DESTINATION bin)
    get_target_property(Qt5_WidgetsLocation Qt5::Widgets LOCATION)
    install(FILES ${Qt5_WidgetsLocation} DESTINATION bin)
    get_target_property(Qt5_OpenGLLocation Qt5::OpenGL LOCATION)
    install(FILES ${Qt5_OpenGLLocation} DESTINATION bin)
endif(MSVC)
