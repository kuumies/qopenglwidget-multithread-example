/**
    @file   opengl_shader.cpp
    @author kuumies <kuumies@gmail.com>
    @brief  Implementation of kuu::opengl::Shader class.
 **/

#include "opengl_shader.h"
#include "opengl.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

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

/* ---------------------------------------------------------------- *
   Returns true if the shader is compiled.
 * ---------------------------------------------------------------- */
bool isShaderCompiled(GLuint shaderId)
{
    GLint status = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    return status == GL_TRUE;
}

/* ---------------------------------------------------------------- *
   Returns uniform location or -1 if the uniform was not found.
 * ---------------------------------------------------------------- */
int uniformLocation(
    GLuint pgm,
    const std::string& uniform)
{
    int uniformLocation = glGetUniformLocation(pgm, uniform.c_str());
    if (uniformLocation == -1)
        std::cerr << "Failed to find "
                  << uniform << " location."
                  << std::endl;
    return uniformLocation;
}

} // anonymous namespace

/* ---------------------------------------------------------------- *
   The data of the shader.
 * ---------------------------------------------------------------- */
struct Shader::Data
{
    // Constructs the shader data.
    Data()
    { createData(); }

    // Destroys the shader data
    ~Data()
    { destroyData(); }

    // Creates the shader data.
    void createData()
    {
        vsh = glCreateShader(GL_VERTEX_SHADER);
        if (vsh == 0)
            std::cerr << "Failed to create vertex shader"
                      << std::endl;

        fsh = glCreateShader(GL_FRAGMENT_SHADER);
        if (fsh == 0)
        {
            std::cerr << "Failed to create fragment shader"
                      << std::endl;
        }

        pgm = glCreateProgram();
        if (pgm == 0)
            std::cerr << "Failed to create shader program"
                      << std::endl;
    }

    // Destroys the shader data. OpenGL resources are freed.
    void destroyData()
    {
        glDeleteShader(vsh);
        glDeleteShader(fsh);
        glDeleteProgram(pgm);
    }

    GLuint vsh = 0; // vertex shader name
    GLuint fsh = 0; // fragment shader name
    GLuint pgm = 0; // shader program name
};

/* ---------------------------------------------------------------- */

Shader::Shader()
    : d(std::make_shared<Data>())
{}

/* ---------------------------------------------------------------- */

void Shader::setVertexShader(const std::string& vertexShader)
{
    const char* vshPtr = vertexShader.c_str();
    glShaderSource(d->vsh, 1, &vshPtr, 0);
    glCompileShader(d->vsh);
    if (!isShaderCompiled(d->vsh))
    {
        std::cerr << "Failed to compile vertex shader" << std::endl;
        std::cerr << shaderInfoLog(d->vsh) << std::endl;
    }
    glAttachShader(d->pgm, d->vsh);
}

/* ---------------------------------------------------------------- */

void Shader::setFragmentShader(const std::string& fragmentShader)
{
    const char* fshPtr = fragmentShader.c_str();
    glShaderSource(d->fsh, 1, &fshPtr, 0);
    glCompileShader(d->fsh);
    if (!isShaderCompiled(d->fsh))
    {
        std::cerr << "Failed to compile fragment shader" << std::endl;
        std::cerr << shaderInfoLog(d->fsh) << std::endl;
    }
    glAttachShader(d->pgm, d->fsh);
}

/* ---------------------------------------------------------------- */

void Shader::link()
{
    glLinkProgram(d->pgm);

    int status = 0;
    glGetProgramiv(d->pgm, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
        std::cerr << "Failed to link shader program" << std::endl;

    glDetachShader(d->pgm, d->vsh);
    glDetachShader(d->pgm, d->fsh);
}

/* ---------------------------------------------------------------- */

void Shader::bind()
{
    glUseProgram(d->pgm);

    glValidateProgram(d->pgm);
    GLint status = 0;
    glGetProgramiv(d->pgm, GL_VALIDATE_STATUS, &status);
    if (status != GL_TRUE)
        std::cerr << "Shader program is not valid" << std::endl;
}

/* ---------------------------------------------------------------- */

void Shader::release()
{
    glUseProgram(0);
}

/* ---------------------------------------------------------------- */

void Shader::setUniform(const std::string& name, int i)
{
    int location = uniformLocation(d->pgm, name);
    glUniform1i(location, i);
}

/* ---------------------------------------------------------------- */

void Shader::setUniform(const std::string& name,
                        const glm::mat4& matrix)
{
    int location = uniformLocation(d->pgm, name);
    glUniformMatrix4fv(location, 1, GL_FALSE,
                       glm::value_ptr(matrix));
}

} // namespace opengl
} // namespace kuu
