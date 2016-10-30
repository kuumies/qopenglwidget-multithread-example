/**
    @file   opengl_viewport_target.h
    @author kuumies <kuumies@gmail.com>
    @brief  Definition of kuu::opengl::ViewportTarget class.
 **/


#pragma once

#include "opengl.h"
#include <memory>

namespace kuu
{
namespace opengl
{

/**
    A viewport target.

    This renders framebuffer texture into viewport.
 **/
class ViewportTarget
{
public:
    /**
        Constructs the viewport target.
        @note OpenGL context must be valid.
     **/
    ViewportTarget();

    /**
        Renders the framebuffer texture into viewport.
        @param textureId The framebuffer texture.
     **/
    void render(GLuint textureId);

private:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
