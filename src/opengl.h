/**
    @file   opengl.h
    @author kuumies <kuumies@gmail.com>
    @brief  Includes OpenGL header that allows usage of OpenGL 3.0.
 **/

#pragma once

#ifdef _WIN32
    #include "glew/glew.h"
#else
    #include <gl3.h>
#endif
