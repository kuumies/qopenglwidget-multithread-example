/* ---------------------------------------------------------------- *
    Author...Kuumies <kuumies@gmail.com>
    Desc.....Definition of kuu::opengl::Mesh class.
 */

#pragma once

#include <memory>
#include <vector>
#include "opengl.h"

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *

   An OpenGL mesh class. This class is using indexed vertex data
   and the vertex and index components can be any type. Below is
   an example how the use the Mesh class to create a quad mesh with
   position and color vertex attributes:

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
 */
class Mesh
{
public:
    /* ------------------------------------------------------------ *
       Constructs the mesh without vertices.
    **/
    Mesh();

    /* ------------------------------------------------------------ *
       Returns the count of written vertex data.
     */
    int vertexCount() const;

    /* ------------------------------------------------------------ *
       Returns the count of written index data.
     */
    int indexCount() const;

    /* ------------------------------------------------------------ *
       Writes a vector of vertex data into device. The existing
       data is overwritten.

       vertexData...The vertex data vector.
     */
    template<typename T>
    void writeVertexData(const std::vector<T>& vertexData)
    {
        writeVertexData(vertexData.size() * sizeof(T),
                        vertexData.size(),
                        vertexData.data());
    }

    /* ------------------------------------------------------------ *
       Writes vertex data into device.

       byteSize.....The size of vertex data in bytes.
       count........The count of vertices in he vertex data.
       vertexData...Unmutable vertex data pointer. The pointer must
                    be valid.
     */
    void writeVertexData(int byteSize, int count,
                         const void* vertexData);

    /* ------------------------------------------------------------ *
       Reads the vector of vertex data from the device.

       vertexData...The vertex data vector. The vector is resized
                    to fit all the written data.
     */
    template<typename T>
    void readVertexData(std::vector<T>& vertexData)
    {
        vertexData.resize(vertexCount());
        readVertexData(vertexData.size() * sizeof(T),
                       vertexData.data());
    }

    /* ------------------------------------------------------------ *
       Reads the vertex data from the device. The data must be
       pre-allocated.

       byteCount...The count of bytes to read from the device.
       data........The vertexData. The data must be pre-allocated.
    */
    void readVertexData(int byteCount, void* data);

    /* ------------------------------------------------------------ *
       Writes a vector of index data into device. The existing
       data is overwritten.

       indexData...The index data to write into device.
     **/
    template<typename T>
    void writeIndexData(const std::vector<T>& indexData)
    {
        writeIndexData(indexData.size() * sizeof(T),
                       indexData.size(),
                       indexData.data());
    }

    /* ------------------------------------------------------------ *
       Writes the index data into device. The existing data is
       overwritten.

       byteSize....The size of the indexData in bytes.
       count.......The count of indices in indexData. This is needed
                   for rendering the mesh.
       indexData...An unmutable array of indices. The pointer must
                   be valid.
     */
    void writeIndexData(int byteSize, int count,
                        const void* indexData);

    /* ------------------------------------------------------------ *
       Reads the vector of index data from the device.

       indexData...The index data vector. The vector is resized
                   to fit all the written data.
     */
    template<typename T>
    void readIndexData(std::vector<T>& indexData)
    {
        indexData.resize(indexCount());
        readIndexData(indexData.size() * sizeof(T),
                      indexData.data());
    }

    /* ------------------------------------------------------------ *
       Reads the index data from the device. The data must be
       pre-allocated.

       byteCount...The count of bytes to read from the device.
       data........The index data. The data pointer must be valid.
    */
    void readIndexData(int byteCount, void* data);

    /* ------------------------------------------------------------ *
       Sets the vertex attribute definition. This will also enable
       the vertex attribute.

       index......The attribute index. This is the index that is
                  set in vertex shader with 'layout' keyword, e.g.
                  'layout (location = 0)' would need index '0'.
       typeSize...The count of components in attribute. A position
                  is usually vec3 containing components x,y,z so the
                  tuple size for it is 3.
       stride.....The byte size of single vertex.
       offset.....The offset of attribute from the start of the
                  vertex, e.g. if the float vertex data is like this
                  in memory: [x][y]|z][u][v][nx][ny][nz] then when
                  settings normal attribute the offset would be
                  6 * sizeof(float).
        type......The data type of the attribute.
     */
    void setAttributeDefinition(
        int index,
        int tupleSize,
        int stride,
        int offset,
        GLenum type = GL_FLOAT);

    /* ------------------------------------------------------------ *
       Binds the mesh into OpenGL context.
     */
    void bind();

    /* ------------------------------------------------------------ *
       Releases the mesh from OpenGL context.
     */
    void release();

    /* ------------------------------------------------------------ *
       Renders the mesh. The mesh must be bound into OpenGL context
       before it can be renderer.

       drawStyle.......The primitive type of the mesh.
       indexDataType...The index data type which was used with
                       setIndexData function.
     */
    void render(GLenum drawStyle = GL_TRIANGLES,
                GLenum indexDataType = GL_UNSIGNED_INT);

private:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
