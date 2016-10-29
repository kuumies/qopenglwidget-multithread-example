/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Definition of kuu::opengl::ViewportTarget class.
 * ---------------------------------------------------------------- */

#pragma once

#include "opengl.h"
#include <memory>
#include <glm/mat4x4.hpp>

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *
   A viewport target. This renders framebuffer texture into viewport.
 * ---------------------------------------------------------------- */
class ViewportTarget
{
public:
    // Defines a shared pointer of quad.
    using Ptr = std::shared_ptr<ViewportTarget>;

    // Constructs the viewport target. OpenGL context must be valid.
    ViewportTarget();

    // Renders the framebuffer texture into viewport.
    void render(GLuint textureId);

private:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
