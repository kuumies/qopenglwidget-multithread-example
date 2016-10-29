/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Implmentation of kuu::opengl::ViewportTarget class.
 * ---------------------------------------------------------------- */

#include "opengl_viewport_target.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

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
   The data of the viewport target.
 * ---------------------------------------------------------------- */
struct ViewportTarget::Data
{
    // Constructs the quad data
    Data()
    { createViewportTarget(); }

    // Destroys the quad data
    ~Data()
    { destroyViewportTarget(); }

    // Creates the viewport target
    //
    void createViewportTarget()
    {
        // -----------------------------------------------------------
        // Create quad vertex data.
        const float w = 1.0f;
        const float h = 1.0f;
        const std::vector<float> vertexData =
        {
          // x   y   z     r     g     b,    u,    v
            -w, -h, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             w, -h, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
             w,  h, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -w,  h, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };

        // -----------------------------------------------------------
        // Create triangle indices (two triangles)

        const std::vector<unsigned int> indexData =
        {
            0u, 1u, 2u,
            2u, 3u, 0u
        };

        // -----------------------------------------------------------
        // Create vertex array object.

        glGenVertexArrays(1, &vao);
        if (vao == 0)
            std::cerr << "Failed to generate VAO" << std::endl;
        glBindVertexArray(vao);

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

        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
        if (current != vao)
            std::cerr << "Failed to bind VAO" << std::endl;

        // -----------------------------------------------------------
        // Set vertex attributes (this will be stored into VAO)

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE,
            8 * sizeof(float), (const GLvoid*) 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE,
            8 * sizeof(float),
            (const GLvoid*) (3 * sizeof(float)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE,
            8 * sizeof(float),
            (const GLvoid*) (6 * sizeof(float)));

        // Release VAO and buffers (notice order)
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // -----------------------------------------------------------
        // Create the vertex shader

        const std::string vshSource =
            "#version 330 core\r\n" // note linebreak
            "layout (location = 0) in vec3 position;"
            "layout (location = 1) in vec3 color;"
            "layout (location = 2) in vec2 texCoord;"
            "out vec4 colorIn;"
            "out vec2 texCoordIn;"
            "void main(void)"
            "{"
               " gl_Position = vec4(position, 1.0);"
                "colorIn = vec4(color, 1.0);"
                "texCoordIn = texCoord;"
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
            "#version 330 core\r\n" // note linebreak
            "uniform sampler2D tex;"
            "in vec4 colorIn;"
            "in vec2 texCoordIn;"
            "out vec4 colorOut;"
            "void main(void)"
            "{"
                "colorOut = texture(tex, texCoordIn);"
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
    void destroyViewportTarget()
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

    GLuint vbo = 0; // vertex buffer object name
    GLuint ibo = 0; // index buffer object name
    GLuint vao = 0; // vertex array object name
    GLuint vsh = 0; // vertex shader name
    GLuint fsh = 0; // fragment shader name
    GLuint pgm = 0; // shader program name
};

/* ---------------------------------------------------------------- *
   Constructs the viewport target.
 * -----------------------------------------------------------------*/
ViewportTarget::ViewportTarget()
    : d(std::make_shared<Data>())
{}

/* ---------------------------------------------------------------- *
   Renders the viewport target.
 * -----------------------------------------------------------------*/
void ViewportTarget::render(GLuint textureId)
{
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Bind the buffers.
    glBindVertexArray(d->vao);

    // Bind and validate the shader program.
    glUseProgram(d->pgm);

    glValidateProgram(d->pgm);
    GLint status = 0;
    glGetProgramiv(d->pgm, GL_VALIDATE_STATUS, &status);
    if (status != GL_TRUE)
        std::cout << "Shader program is not valid" << std::endl;

    const int uniformLocation = glGetUniformLocation(d->pgm, "tex");
    if (uniformLocation == -1)
    {
        std::cerr << "Failed to find tex uniform location."
                  << std::endl;
        return;
    }
    glUniform1i(uniformLocation, 0);

    // Draw the two triangles
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Release the binded state
    glUseProgram(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace opengl
} // namespace kuu
