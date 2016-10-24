/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Definition of kuu::opengl::Quad class.
 * ---------------------------------------------------------------- */

#pragma once

#include <memory>
#include <glm/mat4x4.hpp>

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *
   A quad mesh.

   The mesh is built so that the center of the quad is at the origo.
   Rendering is done with GLSL 3.3 core version. The OpenGL context
   must be valid when the Quad instance is constructed. If the con-
   struction fails then all the errors are printed into standard er-
   ror stream.

   Example:

    // Create quad
    Quad::Ptr quad = std::make_shared<Quad>(3.0f, 4.0f);
    ...
    // update the quad rotation
    quad.update(10); // 10 milliseconds
    ...
    // render the quad into currently bound framebuffer.
    glm::mat4 cameraViewMatrix       = getCameraViewMatrix();
    glm::mat4 cameraProjectionMatrix = getCameraProjectionMatrix();
    quad.render(cameraViewMatrix, cameraProjectionMatrix);

 * ---------------------------------------------------------------- */
class Quad
{
public:
    // Defines a shared pointer of quad.
    using Ptr = std::shared_ptr<Quad>;

    // Constructs the quad. OpenGL context must be valid.
    Quad(float width = 1.0f, float height = 1.0f);

    // Updates the quad rotation.
    void update(float elapsed);

    // Renders the quad
    void render(const glm::mat4& view,
                const glm::mat4& projection);

private:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
