/**
    @file   opengl_shader.h
    @author kuumies <kuumies@gmail.com>
    @brief  Definition of kuu::opengl::Shader class.
 **/

#pragma once

#include <glm/mat4x4.hpp>
#include <memory>
#include <string>

namespace kuu
{
namespace opengl
{

/**
    An OpenGL shader class. This class is combined container of
    vertex and fragment shader and of program that consist of
    those shaders. The below is an example how to create an shader
    from vertex and fragment source strings.

    @code

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
    shader->bind();
    shader->setUniform("cameraMatrix", glm::mat4(1.0f));
    shader->release();

    @endcode
 **/
class Shader
{
public:
    /**
        Constructs the shader.

        The shader is not valid until vertex and fragment shader
        sources are set and the shader is linked.
     **/
    Shader();

    /**
        Sets the vertex shader source.
        @param vertexShader The vertex shader source.
     **/
    void setVertexShader(const std::string& vertexShader);

    /**
        Sets the fragment shader source.
        @param fragmentShader The fragment shader source.
     **/
    void setFragmentShader(const std::string& fragmentShader);

    /**
        Links the shader.

        This will link the vertex and fragment shader into program.
     **/
    void link();

    /**
        Binds the shader into OpenGL context.
     **/
    void bind();

    /**
        Releases the shader from the OpenGL context.
     **/
    void release();

    /**
        Sets an integer uniform.
        @param name Uniform name.
        @param i    Integer value.
     **/
    void setUniform(const std::string& name, int i);

    /**
        Sets a 4x4 matrix uniform.
        @param name   Uniform name.
        @param matrix Matrix value.
     */
    void setUniform(const std::string& name, const glm::mat4& matrix);

private:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
