/**
    @file   opengl_quad.h
    @author kuumies <kuumies@gmail.com>
    @brief  Definition of kuu::opengl::Quad class.
 **/

#pragma once

#include <memory>
#include <glm/mat4x4.hpp>

namespace kuu
{
namespace opengl
{

/**
    A quad mesh.

    The mesh is built so that the center of the quad is at the origo.
    Rendering is done with GLSL 3.3 core version. The OpenGL context
    must be valid when the Quad instance is constructed. If the con-
    struction fails then all the errors are printed into standard er-
    ror stream.

    @code

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

    @endcode

 **/
class Quad
{
public:
    /**
        Constructs the quad from the width and height dimensions.
        @param width  The width of the quad.
        @param height The height of the quad.
     **/
    Quad(float width = 1.0f, float height = 1.0f);

    /**
        Updates the quad rotation around Y-axis.
        @param elapsed Time in milliseconds since the function was
                       called last time.
     **/
    void update(float elapsed);

    /**
        Renders the quad.

        The input view and projection matrics are used to transform
        the vertices from world space into camera clipping space.

        @param view       The view matrix that transforms vertex from
                          world space into view space.
        @param projection The projection matrix that transforms vertex
                          from view space into clipped camera space.
     **/
    void render(const glm::mat4& view,
                const glm::mat4& projection);

private:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
