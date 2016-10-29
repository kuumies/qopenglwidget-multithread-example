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
   An OpenGL widget. The widget shows a framebuffer that was created
   and update in rendering thread.
 * ---------------------------------------------------------------- */
class Widget
    : public QOpenGLWidget
    , public std::enable_shared_from_this<Widget>
{
public:
    // Defines a shared pointer of the widget.
    using Ptr = std::shared_ptr<Widget>;

    // Constructs the widget.
    Widget();

    // Starts the rendering thread
    void startThread();
    // Stops the rendering thread
    void stopThread();

protected:
    void paintGL();
    void closeEvent(QCloseEvent* e);

private:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
