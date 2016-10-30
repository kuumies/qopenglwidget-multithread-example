/**
    @file   opengl_mesh.h
    @author kuumies <kuumies@gmail.com>
    @brief  Definition of kuu::opengl::Mesh class.
 **/

#pragma once

#include <memory>
#include <vector>
#include "opengl.h"

namespace kuu
{
namespace opengl
{

/**
    An OpenGL mesh class.

    To transfer mesh vertices and indices from CPU memory into GPU
    memory so that a vertex shader can access the data one would need
    to create vertex and index buffer objects and a vertex array
    object to store the vertex attribute definitions and the bound
    buffer states.

    This class is intended to ease that so that user needs to just
    write the vectors of vertex and index data and describe the vertex
    data definition.

    Below is an example how the use the Mesh class to create a quad
    mesh with position and color vertex attributes:

    @code
    const float w = 1.0f;
    const float h = 1.0f;
    const std::vector<float> vertexData =
    {
      // x   y   z     r     g     b
        -w, -h, 0.0f, 1.0f, 0.0f, 0.0f,
         w, -h, 0.0f, 0.0f, 1.0f, 0.0f,
         w,  h, 0.0f, 0.0f, 0.0f, 1.0f,
        -w,  h, 0.0f, 1.0f, 1.0f, 0.0f
    };

    const std::vector<unsigned int> indexData =
    {
        0u, 1u, 2u,
        2u, 3u, 0u
    };

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
    mesh->writeVertexData(vertexData);
    mesh->writeIndexData(indexData);
    mesh->setAttributeDefinition(0, 3, 6 * sizeof(float), 0);
    mesh->setAttributeDefinition(1, 3, 6 * sizeof(float),
                                 3 * sizeof(float));
    @endcode
 **/
class Mesh
{
public:
    /**
        Constructs the mesh without vertices.
     **/
    Mesh();

    /**
       Writes a vector of vertex data into device.

       @note The existing data is overwritten.
       @param vertexData The vertex data vector.
     */
    template<typename T>
    void writeVertexData(const std::vector<T>& vertexData)
    {
        writeVertexData(vertexData.size() * sizeof(T),
                        vertexData.size(),
                        vertexData.data());
    }

    /**
       Writes vertex data into device.

       @param byteSize...The size of vertex data in bytes.
       @param count......The count of vertices in he vertex data.
       @param vertexData Unmutable vertex data pointer. The pointer
                         must be valid.
     */
    void writeVertexData(int byteSize, int count,
                         const void* vertexData);

    /**
       Reads the vector of vertex data from the device.

       @param vertexData The vertex data vector. The vector is resized
                         to fit all the written data.
     */
    template<typename T>
    void readVertexData(std::vector<T>& vertexData)
    {
        vertexData.resize(vertexCount());
        readVertexData(vertexData.size() * sizeof(T),
                       vertexData.data());
    }

    /**
       Reads the vertex data from the device.

       @note The data must be pre-allocated.

       @param byteCount The count of bytes to read from the device.
       @param data      The vertexData. The data must be pre-allocated.
    */
    void readVertexData(int byteCount, void* data);

    /**
        Returns the count of written vertex data.
        @return Returns the count of vetices that has been written
                into device.
     **/
    int vertexCount() const;

    /**
       Writes a vector of index data into device.

       @note The existing data is overwritten.

       @param indexData The index data to write into device.
     **/
    template<typename T>
    void writeIndexData(const std::vector<T>& indexData)
    {
        writeIndexData(indexData.size() * sizeof(T),
                       indexData.size(),
                       indexData.data());
    }

    /**
       Writes the index data into device.

       @note The existing data is overwritten.

       @param byteSize  The size of the indexData in bytes.
       @param count     The count of indices in indexData.
       @param indexData An unmutable array of indices.
     **/
    void writeIndexData(int byteSize, int count,
                        const void* indexData);

    /**
       Reads index data from the device into vector.

       @param indexData The index data vector. The vector is resized
                        to fit all the written data.
     **/
    template<typename T>
    void readIndexData(std::vector<T>& indexData)
    {
        indexData.resize(indexCount());
        readIndexData(indexData.size() * sizeof(T),
                      indexData.data());
    }

    /**
       Reads the index data from the device.

       @note The @ref data must be pre-allocated.

       @param byteCount The count of bytes to read from the device.
       @param data      The index data. The pointer must be valid and
                        contain enough memory to hold all the indices.
    **/
    void readIndexData(int byteCount, void* data);

    /**
       Returns the component count of written index data.
       @return Returns the count of indices that has been written
               into device.
     **/
    int indexCount() const;

    /**
        Sets the vertex attribute definition.

        Vertex attribute needs to be defined and enabled so that the
        vertex shader knows how to access it from vertex buffer.

        @param index    The attribute index. This is the index that is
                        set in vertex shader with @c layout keyword,
                        e.g.
                        @c layout (location = 0) would need index @c 0.
        @param typeSize The count of components in attribute. A
                        position is usually @c vec3 containing
                        components x,y,z so the tuple size for it is
                        @c 3.
        @param stride   The byte size of single vertex.
        @param offset   The offset of attribute from the start of the
                        vertex, e.g. if the float vertex data is like
                        this in memory: @c [x][y]|z][u][v][nx][ny][nz]
                        then when settings normal attribute the offset
                        would be @c 6*sizeof(float).
        @param type     The data type of the attribute.
     **/
    void setAttributeDefinition(
        int index,
        int tupleSize,
        int stride,
        int offset,
        GLenum type = GL_FLOAT);

    /**
        Binds the mesh into OpenGL context.
     **/
    void bind();

    /**
        Releases the mesh from OpenGL context.
     **/
    void release();

    /**
        Renders the mesh.

        Renders the mesh with the given in drawing style and index
        data type. The type must be the same that was used when the
        index data was set with @ref writeIndexData function.

        The mesh must be bound into OpenGL context by calling @ref
        bind before the mesh can be rendered.

        @param drawStyleThe  OpenGL primitive type of the mesh, e.g.
                             for triangle mesh this would be @c
                             GL_TRIANGLES, for points @c GL_POINTS
                             etc.

        @param indexDataType Index data type which was used with @ref
                             @ref setIndexData function, e.g. with @c
                             uint data type this would be @c
                             GL_UNSIGNED_INT.
     **/
    void render(GLenum drawStyle = GL_TRIANGLES,
                GLenum indexDataType = GL_UNSIGNED_INT);

private:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
