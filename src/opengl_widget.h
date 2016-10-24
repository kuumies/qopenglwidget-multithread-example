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
    Q_OBJECT

public:
    // Defines a shared pointers of widget.
    using Ptr     = std::shared_ptr<Widget>;
    using WeakPtr = std::weak_ptr<Widget>;

    // Constructs the widget.
    Widget();

    // Starts the rendering thread.
    void startThread();

    // Stops the rendering thread.
    void stopThread();

signals:
    // Request renderer to draw the next frame.
    void renderRequest();

private slots:
    void onWidgetAboutToCompose();
    void onWidgetFrameSwapped();
    void onWidgetAboutToResize();
    void onWidgetResized();
    void onRendererRequestContext();

protected:
    void paintEvent(QPaintEvent* e);
    void closeEvent(QCloseEvent* e);

private:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
