/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Implementation of kuu::opengl::Mesh class.
 */

#include "opengl_mesh.h"
#include <iostream>

namespace kuu
{
namespace opengl
{

namespace
{

/* ---------------------------------------------------------------- *
   Returns true if the given in vertex array object is currently
   bound into OpenGL context.
 */
bool isVertexArrayCurrent(GLuint vao)
{
    GLint current = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
    return current == vao;
}

/* ---------------------------------------------------------------- *
   Returns true if the given in vertex buffer object is currently
   bound into OpenGL context.
 */
bool isVertexBufferCurrent(GLuint vbo)
{
    GLint current = 0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &current);
    return current == vbo;
}

/* ---------------------------------------------------------------- *
   Returns true if the given in index buffer object is currently
   bound into OpenGL context.
 */
bool isIndexBufferCurrent(GLuint ibo)
{
    GLint current = 0;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &current);
    return current == ibo;
}

} // anonymous namespace

/* ---------------------------------------------------------------- *
   The data of the mesh.
 */
struct Mesh::Data
{
    // Constructs the mesh data.
    Data()
    { createData(); }

    // Destroys the mesh data
    ~Data()
    { destroyData(); }

    // Creates the initial mesh data.
    void createData()
    {
        // Create vertex array object.
        glGenVertexArrays(1, &vao);
        if (vao == 0)
            std::cerr << "Failed to generate VAO" << std::endl;

        // Create vertex buffer object.
        glGenBuffers(1, &vbo);
        if (vbo == 0)
            std::cerr << "Failed to generate VBO" << std::endl;

        // Create index buffer object
        glGenBuffers(1, &ibo);
        if (ibo == 0)
            std::cerr << "Failed to generate IBO" << std::endl;
    }

    // Destroys the mesh. OpenGL resources are freed.
    void destroyData()
    {
        // Destroy vertex and index buffers
        glDeleteBuffers(1, &ibo);
        glDeleteBuffers(1, &vbo);
        // Destroy vertex array
        glDeleteVertexArrays(1, &vao);
    }

    GLuint vbo = 0; // vertex buffer object name
    GLuint ibo = 0; // index buffer object name
    GLuint vao = 0; // vertex array object name
    GLuint indexCount  = 0;
    GLuint vertexCount = 0;
};

/* ---------------------------------------------------------------- */

Mesh::Mesh()
    : d(std::make_shared<Data>())
{}

/* ---------------------------------------------------------------- */

int Mesh::vertexCount() const
{
    return d->vertexCount;
}

/* ---------------------------------------------------------------- */

int Mesh::indexCount() const
{
    return d->indexCount;
}

/* ---------------------------------------------------------------- */

void Mesh::writeVertexData(int byteSize,
                           int count,
                           const void* vertexData)
{
    glBindVertexArray(d->vao);
    if (!isVertexArrayCurrent(d->vao))
        std::cerr << "Failed to bind VAO" << std::endl;

    glBindBuffer(GL_ARRAY_BUFFER, d->vbo);
    if (!isVertexBufferCurrent(d->vbo))
        std::cerr << "Failed to bind VBO" << std::endl;

    d->vertexCount = count;
    glBufferData(GL_ARRAY_BUFFER,
                 byteSize,
                 vertexData,
                 GL_STATIC_DRAW);

    glBindVertexArray(0); // VAO before VBO!
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* ---------------------------------------------------------------- */

void Mesh::readVertexData(int byteCount, void* data)
{
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, byteCount, data);
}

/* ---------------------------------------------------------------- */

void Mesh::writeIndexData(int byteSize,
                          int count,
                          const void* indexData)
{
    glBindVertexArray(d->vao);
    if (!isVertexArrayCurrent(d->vao))
        std::cerr << "Failed to bind VAO" << std::endl;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->ibo);
    if (!isIndexBufferCurrent(d->ibo))
        std::cerr << "Failed to bind IBO" << std::endl;

    d->indexCount = count;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 byteSize,
                 indexData,
                 GL_STATIC_DRAW);

    glBindVertexArray(0); // VAO before IBO!
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::readIndexData(int byteCount, void* data)
{
    glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, byteCount, data);
}

/* ---------------------------------------------------------------- */

void Mesh::setAttributeDefinition(int index,
                                  int tupleSize,
                                  int stride,
                                  int offset,
                                  GLenum type)
{
    glBindVertexArray(d->vao);
    if (!isVertexArrayCurrent(d->vao))
        std::cerr << "Failed to bind VAO" << std::endl;

    glBindBuffer(GL_ARRAY_BUFFER, d->vbo);
    if (!isVertexBufferCurrent(d->vbo))
        std::cerr << "Failed to bind VBO" << std::endl;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->ibo);
    if (!isIndexBufferCurrent(d->ibo))
        std::cerr << "Failed to bind IBO" << std::endl;

    glEnableVertexAttribArray(index);
    glVertexAttribPointer(
        index, tupleSize, type, GL_FALSE,
        stride,
        (const GLvoid*) offset);

    glBindVertexArray(0); // VAO before VBO and IBO!
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/* ---------------------------------------------------------------- */

void Mesh::bind()
{
    glBindVertexArray(d->vao);
    if (!isVertexArrayCurrent(d->vao))
        std::cerr << "Failed to bind VAO" << std::endl;
}

/* ---------------------------------------------------------------- */

void Mesh::release()
{
    glBindVertexArray(0);
}

/* ---------------------------------------------------------------- */

void Mesh::render(GLenum drawStyle, GLenum indexDataType)
{
    glDrawElements(drawStyle, d->indexCount, indexDataType, 0);
}

} // namespace opengl
} // namespace kuu
