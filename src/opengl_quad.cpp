/**
    @file   opengl_quad.cpp
    @author kuumies <kuumies@gmail.com>
    @brief  Implementation of kuu::opengl::Quad class.
 **/

#include "opengl_quad.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "opengl.h"
#include "opengl_mesh.h"
#include "opengl_shader.h"

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *
   The data of the quad.
 * ---------------------------------------------------------------- */
struct Quad::Data
{
    // Constructs the quad data
    Data(float width, float height)
        : width(width)
        , height(height)
    { createQuad(); }

    // Creates the quad. This will create a vertex buffer with two
    // triangles where a single vertex contains position and color.
    // The vertices and triangle indices are written into OpenGL
    // buffers. Vertex array is used to store the vertex attribute
    // information.
    //
    // A simple shader is used to transform vertices from model space
    // into camera clip space. The shading is done with the vertex
    // colors.
    //
    // If any of the OpenGL functions fails then the failed object
    // is written into standard error stream. One failure leads to
    // rendering to fail.
    //
    void createQuad()
    {
        // -----------------------------------------------------------
        // Create quad vertex data. The center of the quad is at the
        // origo. The vertex properties are packed where the first
        // is vertes position and then color components.

        const float w = width  * 0.5f;
        const float h = height * 0.5f;
        const std::vector<float> vertexData =
        {
          // x   y   z     r     g     b
            -w, -h, 0.0f, 1.0f, 0.0f, 0.0f,
             w, -h, 0.0f, 0.0f, 1.0f, 0.0f,
             w,  h, 0.0f, 0.0f, 0.0f, 1.0f,
            -w,  h, 0.0f, 1.0f, 1.0f, 0.0f
        };

        // -----------------------------------------------------------
        // Create triangle indices (two triangles)

        const std::vector<unsigned int> indexData =
        {
            0u, 1u, 2u,
            2u, 3u, 0u
        };

        mesh = std::make_shared<Mesh>();
        mesh->writeVertexData(vertexData);
        mesh->writeIndexData(indexData);
        mesh->setAttributeDefinition(0, 3, 6 * sizeof(float), 0);
        mesh->setAttributeDefinition(1, 3, 6 * sizeof(float),
                                     3 * sizeof(float));

        // -----------------------------------------------------------
        // Create the shader

        const std::string vshSource =
            "#version 330 core\r\n" // note linebreak
            "layout (location = 0) in vec3 position;"
            "layout (location = 1) in vec3 color;"
            "uniform mat4 cameraMatrix;"
            "out vec4 colorIn;"
            "void main(void)"
            "{"
               " gl_Position = cameraMatrix * vec4(position, 1.0);"
                "colorIn = vec4(color, 1.0);"
            "}";

        const std::string fshSource =
            "#version 330 core\r\n" // note linebreak
            "in vec4 colorIn;"
            "out vec4 colorOut;"
            "void main(void)"
            "{"
                "colorOut = colorIn;"
            "}";

        shader = std::make_shared<Shader>();
        shader->setVertexShader(vshSource);
        shader->setFragmentShader(fshSource);
        shader->link();
    }

    float width  = 1.0f; // width of the quad
    float height = 1.0f; // height of the quad

    glm::quat yaw; // rotation around y-axis

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Shader> shader;
};

/* ---------------------------------------------------------------- */

Quad::Quad(float width, float height)
    : d(std::make_shared<Data>(width, height))
{}

/* ---------------------------------------------------------------- */

void Quad::update(float elapsed)
{
    const float angleChangePerMillisecond = 180.0f/1000.0f;
    const float angleChange = angleChangePerMillisecond * elapsed;
    d->yaw *= glm::angleAxis(
                    glm::radians(angleChange),
                    glm::vec3(0.0f, 1.0f, 0.0f));
}

/* ---------------------------------------------------------------- */

void Quad::render(const glm::mat4& view,
                  const glm::mat4& projection)
{
    // Creates the transform from model space into world space
    const glm::mat4 model = glm::mat4_cast(d->yaw);
    const glm::mat4 cameraMatrix = projection * view * model;

    d->mesh->bind();
    d->shader->bind();
    d->shader->setUniform("cameraMatrix", cameraMatrix);
    d->mesh->render(GL_TRIANGLES);
    d->shader->release();
    d->mesh->release();
}

} // namespace opengl
} // namespace kuu
