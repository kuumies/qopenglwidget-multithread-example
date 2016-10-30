/**
   @file   opengl_widget.cpp
   @author kuumies <kuumies@gmail.com>
   @brief  Implementation of kuu::opengl::Widget class.
 **/

#include "opengl_widget.h"
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
    std::shared_ptr<RenderingThread> renderingThread;
    std::shared_ptr<ViewportTarget> viewportTarget;
};

/* ---------------------------------------------------------------- */

Widget::Widget()
    : d(std::make_shared<Data>())
{}

/* ---------------------------------------------------------------- */

void Widget::startThread()
{
    if (d->renderingThread)
        stopThread();

    d->renderingThread = std::make_shared<RenderingThread>(this);
    d->renderingThread->start();
}

/* ---------------------------------------------------------------- */

void Widget::stopThread()
{
    if (d->renderingThread && d->renderingThread->isRunning())
    {
        d->renderingThread->stop();
        d->renderingThread->quit();
        d->renderingThread->wait();
    }
    d->renderingThread.reset();
}

/* ---------------------------------------------------------------- */

void Widget::paintGL()
{
    if (!d->renderingThread)
        return;

    if (!d->viewportTarget)
        d->viewportTarget = std::make_shared<ViewportTarget>();

    d->renderingThread->lock();
    const GLuint textureId =
        d->renderingThread->framebufferTexture();
    d->viewportTarget->render(textureId);
    d->renderingThread->unlock();
}

/* ---------------------------------------------------------------- */

void Widget::closeEvent(QCloseEvent* /*e*/)
{
    stopThread();
}

} // namespace opengl
} // namespace kuu
