/**
   @file   opengl_widget.h
   @author kuumies <kuumies@gmail.com>
   @brief  Definition of kuu::opengl::Widget class.
 **/

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

/**
    An widget with OpenGL rendering capabilities.

    The widget will render an offscreen framebuffer into screen that
    was created by a rendering thread. The framebuffer size matches
    the widget size when the thread was started.

    Below is a code snipped how to create the widget with correct
    surface format.

    @code
    QSurfaceFormat format;
    format.setDepthBufferSize(16);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    std::shared_ptr<Widget> widget =
        std::make_shared<Widget>();
    widget->show();
    widget->startThread();
    @endcode

    @note The rendering thread is automatically stopped when the
          widget is closed.
 **/
class Widget : public QOpenGLWidget
{
public:
    /**
        Constructs the widget.

        The rendering thread is not started until @ref startThread is
        called.
     **/
    Widget();

    /**
        Starts the rendering thread.

        If the thread is already running then it is stopped and a
        new thread is created.
     **/
    void startThread();

    /**
        Stops the rendering thread.

        The function will wait until the thread really quits. If the
        thread is not running then the function just returns.
     **/
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
