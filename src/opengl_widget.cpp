/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Implementation of kuu::opengl::Widget class.
 * ---------------------------------------------------------------- */

#include "opengl_widget.h"
#include "opengl.h"
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtGui/QOffscreenSurface>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include "opengl_rendering_thread.h"
#include "opengl_viewport_target.h"

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *
   The data of the widget.
 * ---------------------------------------------------------------- */
struct Widget::Data
{
    // Thread that does the triangle rendering.
    RenderingThread::Ptr renderingThread;
    // Viewport that draws the framebuffer rendered in
    // rendering thread.
    ViewportTarget::Ptr viewportTarget;
};

/* ---------------------------------------------------------------- *
   Constructs the widget.
 * -----------------------------------------------------------------*/
Widget::Widget()
    : d(std::make_shared<Data>())
{}

/* ---------------------------------------------------------------- *
   Starts the rendering thread.
 * -----------------------------------------------------------------*/
void Widget::startThread()
{
    if (d->renderingThread)
        return;
    d->renderingThread = std::make_shared<RenderingThread>(this, size());
    d->renderingThread->moveToThread(d->renderingThread.get());
    d->renderingThread->start();
}

/* ---------------------------------------------------------------- *
   Stops the rendering thread.
 * -----------------------------------------------------------------*/
void Widget::stopThread()
{
    if (!d->renderingThread)
        return;

    if (d->renderingThread->isRunning())
    {
        d->renderingThread->stop();
        d->renderingThread->quit();
        d->renderingThread->wait();
    }
    d->renderingThread.reset();
}

void Widget::paintGL()
{
    // Wait till the rendering thread is created.
    if (!d->renderingThread)
        return;

    // Create the viewport target if not done already
    if (!d->viewportTarget)
        d->viewportTarget = std::make_shared<ViewportTarget>();

    // Get the framebuffer texture from the rendering thread.
    d->renderingThread->lock();
    const GLuint textureId = d->renderingThread->framebufferTex();

    // Draw the framebuffer texture.
    d->viewportTarget->render(textureId);

    // Release the rendering.
    d->renderingThread->unlock();
}

/* ---------------------------------------------------------------- *
   Stop the thread during exit.
 * -----------------------------------------------------------------*/
void Widget::closeEvent(QCloseEvent* /*e*/)
{
    stopThread();
}

} // namespace opengl
} // namespace kuu
