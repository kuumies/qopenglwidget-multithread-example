/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Definition of kuu::opengl::Widget class.
 * ---------------------------------------------------------------- */

#pragma once

#include <memory>
#ifdef _WIN32
    #include "opengl.h"
    #include <QtWidgets/QOpenGLWidget>
#else
    #include <QtWidgets/QOpenGLWidget>
    #include "opengl.h"
#endif

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *
   An OpenGL widget.
 * ---------------------------------------------------------------- */
class Widget
    : public QOpenGLWidget
    , public std::enable_shared_from_this<Widget>
{
public:
    // Defines a shared pointer of widget.
    using Ptr = std::shared_ptr<Widget>;

    // Constructs the widget.
    Widget();

private:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
