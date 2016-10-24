/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Implmentation of kuu::opengl::Quad class.
 * ---------------------------------------------------------------- */

#include "opengl_quad.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "opengl.h"

namespace kuu
{
namespace opengl
{

namespace
{

/* ---------------------------------------------------------------- *
   Returns the OpenGL shader info log 
 * ---------------------------------------------------------------- */
std::string shaderInfoLog(GLint id)
{
    GLint length = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

    if (length <= 0)
        return std::string();

    std::string log;
    log.resize(length + 1);
    glGetShaderInfoLog(id, length, NULL, (GLchar*)log.c_str());

    log.erase(std::remove(log.begin(), log.end(), '\0'), log.end());
    return log;
}
    
} // anonymous namespace

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

    // Destroys the quad data
    ~Data()
    { destroyQuad(); }

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

        // -----------------------------------------------------------
        // Create the OpenGL vertex buffer object and write the
        // vertices into it (ID and bind statuses are asserted).

        glGenBuffers(1, &vbo);
        if (vbo == 0)
            std::cerr << "Failed to generate VBO" << std::endl;

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        GLint current = 0;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &current);
        if (current != vbo)
            std::cerr << "Failed to bind VBO" << std::endl;

        glBufferData(GL_ARRAY_BUFFER,
                     vertexData.size() * sizeof(float),
                     &vertexData[0],
                     GL_STATIC_DRAW);

        // -----------------------------------------------------------
        // Create index buffer object and writes the indices into it.

        glGenBuffers(1, &ibo);
        if (ibo == 0)
            std::cerr << "Failed to generate IBO" << std::endl;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &current);
        if (current != ibo)
            std::cerr << "Failed to bind IBO" << std::endl;

        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indexData.size() * sizeof(unsigned int),
                     &indexData[0],
                     GL_STATIC_DRAW);

        // -----------------------------------------------------------
        // Create vertex array object and set the vertex attributes
        // (position and color) definitions. The attributes will be
        // remembered when the object is bind (note that buffer
        // objects are still bound into OpenGL context).

        glGenVertexArrays(1, &vao);
        if (vao == 0)
            std::cerr << "Failed to generate VAO" << std::endl;

        glBindVertexArray(vao);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
        if (current != vao)
            std::cerr << "Failed to bind VAO" << std::endl;

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE,
            6 * sizeof(float), (const GLvoid*) 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE,
            6 * sizeof(float),
            (const GLvoid*) (3 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // -----------------------------------------------------------
        // Create the vertex shader

        const std::string vshSource =
            "#version 330 core \n" // note linebreak
            "layout (location = 0) in vec3 position;"
            "layout (location = 1) in vec3 color;"
            "uniform mat4 cameraMatrix;"
            "out vec4 colorIn;"
            "void main(void)"
            "{"
               " gl_Position = cameraMatrix * vec4(position, 1.0);"
                "colorIn = vec4(color, 1.0);"
            "}";

        vsh = glCreateShader(GL_VERTEX_SHADER);
        if (vsh == 0)
            std::cerr << "Failed to create vertex shader"
                      << std::endl;

        const char* vshPtr = vshSource.c_str();
        glShaderSource(vsh, 1, &vshPtr, 0);

        glCompileShader(vsh);
        GLint status = 0;
        glGetShaderiv(vsh, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            std::cerr << "Failed to compile vertex shader"
                      << std::endl;
            std::cerr << shaderInfoLog(vsh) << std::endl;
        }

        // -----------------------------------------------------------
        // Create the fragment shader.

        const std::string fshSource =
            "#version 330 core \n" // note linebreak
            "in vec4 colorIn;"
            "out vec4 colorOut;"
            "void main(void)"
            "{"
                "colorOut = colorIn;"
            "}";

        fsh = glCreateShader(GL_FRAGMENT_SHADER);
        if (fsh == 0)
        {
            std::cerr << "Failed to create fragment shader"
                      << std::endl;
            std::cerr << shaderInfoLog(fsh) << std::endl;
        }

        const char* fshPtr = fshSource.c_str();
        glShaderSource(fsh, 1, &fshPtr, 0);

        glCompileShader(fsh);
        glGetShaderiv(fsh, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
            std::cerr << "Failed to compile fragment shader"
                      << std::endl;

        // -----------------------------------------------------------
        // Create the OpenGL shader program.

        pgm = glCreateProgram();
        if (pgm == 0)
            std::cerr << "Failed to create shader program"
                      << std::endl;

        glAttachShader(pgm, vsh);
        glAttachShader(pgm, fsh);

        glLinkProgram(pgm);
        glGetProgramiv(pgm, GL_LINK_STATUS, &status);
        if (status != GL_TRUE)
            std::cerr << "Failed to ling shader program"
                      << std::endl;
    }

    // Destroys the quad. OpenGL resources are freed.
    void destroyQuad()
    {
        // Destroy vertex and index buffers
        glDeleteBuffers(1, &ibo);
        glDeleteBuffers(1, &vbo);
        // Destroy vertex array
        glDeleteVertexArrays(1, &vao);
        // Destroy shader
        glDeleteShader(vsh);
        glDeleteShader(fsh);
        glDeleteProgram(pgm);
    }

    float width  = 1.0f; // width of the quad
    float height = 1.0f; // height of the quad

    GLuint vbo = 0; // vertex buffer object name
    GLuint ibo = 0; // index buffer object name
    GLuint vao = 0; // vertex array object name
    GLuint vsh = 0; // vertex shader name
    GLuint fsh = 0; // fragment shader name
    GLuint pgm = 0; // shader program name

    glm::quat yaw; // rotation around y-axis
};

/* ---------------------------------------------------------------- *
   Constructs the quad from the width and height dimensions.
 * -----------------------------------------------------------------*/
Quad::Quad(float width, float height)
    : d(std::make_shared<Data>(width, height))
{}

/* ---------------------------------------------------------------- *
   Updates the quad rotation around Y-axis
 * -----------------------------------------------------------------*/
void Quad::update(float elapsed)
{
    const float angleChangePerMillisecond = 180.0f/1000.0f;
    const float angleChange = angleChangePerMillisecond * elapsed;
    d->yaw *= glm::angleAxis(
                    glm::radians(angleChange),
                    glm::vec3(0.0f, 1.0f, 0.0f));
}

/* ---------------------------------------------------------------- *
   Renders the quad. The input view and projection matrics are
   used to transform the vertices from world space into camera
   clipping space.
 * -----------------------------------------------------------------*/
void Quad::render(const glm::mat4& view,
                  const glm::mat4& projection)
{
    // Bind the buffers.
    glBindVertexArray(d->vao);
    glBindBuffer(GL_ARRAY_BUFFER, d->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->ibo);

    // Bind and validate the shader program.
    glUseProgram(d->pgm);

    glValidateProgram(d->pgm);
    GLint status = 0;
    glGetProgramiv(d->pgm, GL_VALIDATE_STATUS, &status);
    if (status != GL_TRUE)
        std::cout << "Shader program is not valid" << std::endl;

    // Creates the transform from model space into world space
    glm::mat4 model;
    model = glm::mat4_cast(d->yaw);

    // Set the camera matrix
    const glm::mat4 camera = projection * view * model;
    const int uniformLocation =
        glGetUniformLocation(d->pgm, "cameraMatrix");
    if (uniformLocation == -1)
    {
        std::cerr << "Failed to find cameraMatrix uniform location."
                  << std::endl;
        return;
    }
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE,
                       glm::value_ptr(camera));

    // Draw the two triangles
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Release the binded state
    glUseProgram(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

} // namespace opengl
} // namespace kuu
