/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Implementation of kuu::opengl::Widget class.
 * ---------------------------------------------------------------- */

#include "opengl_widget.h"
#include <QtCore/QThread>
#include <QtGui/QOpenGLContext>
#include "opengl_renderer_object.h"

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *
   The data of the widget.
 * ---------------------------------------------------------------- */
struct Widget::Data
{
    RendererObject::Ptr renderer;
    std::shared_ptr<QThread> thread;
};

/* ---------------------------------------------------------------- *
   Constructs the widget.
 * -----------------------------------------------------------------*/
Widget::Widget()
    : d(std::make_shared<Data>())
{
    connect(this, &QOpenGLWidget::aboutToCompose,
            this, &Widget::onWidgetAboutToCompose);

    connect(this, &QOpenGLWidget::frameSwapped,
            this, &Widget::onWidgetFrameSwapped);

    connect(this, &QOpenGLWidget::aboutToResize,
            this, &Widget::onWidgetAboutToResize);

    connect(this, &QOpenGLWidget::resized,
            this, &Widget::onWidgetResized);
}

/* ---------------------------------------------------------------- *
   Starts the rendering thread.
 * -----------------------------------------------------------------*/
void Widget::startThread()
{
    if (d->thread)
        stopThread();
    d->thread = std::make_shared<QThread>();

    d->renderer = std::make_shared<RendererObject>(shared_from_this());
    d->renderer->moveToThread(d->thread.get());

    connect(this,              &Widget::renderRequest,
            d->renderer.get(), &RendererObject::render);

    connect(d->renderer.get(), &RendererObject::requestContext,
            this,              &Widget::onRendererRequestContext);

    d->thread->start();
}

/* ---------------------------------------------------------------- *
   Stops the rendering thread.
 * -----------------------------------------------------------------*/
void Widget::stopThread()
{
    if (!d->thread)
        return;

    d->renderer->stop();
    if (d->thread->isRunning())
    {
        d->thread->quit();
        d->thread->wait();
    }
    d->thread.reset();

    d->renderer.reset();
}

/* ---------------------------------------------------------------- *
   QOpenGLWidget is starting the framebuffer composition. During
   this time the thread must not make the OpenGL context current
   from it's thread untill the framebuffer is swapped (see
   onWidgetFrameSwapped() function.
 * -----------------------------------------------------------------*/
void Widget::onWidgetAboutToCompose()
{
    if (d->renderer)
        d->renderer->lockRenderer();
}

/* ---------------------------------------------------------------- *
   QOpenGLWidget has swapped the framebuffers, the rendering thread
   can access the OpenGL context once again.
 * -----------------------------------------------------------------*/
void Widget::onWidgetFrameSwapped()
{
    if (d->renderer)
    {
        d->renderer->unlockRenderer();
        emit renderRequest();
    }
}

/* ---------------------------------------------------------------- *
   Stop the rendering when user starts to resize the widget. This
   seems to block the rendering for maybe too long...
 * -----------------------------------------------------------------*/
void Widget::onWidgetAboutToResize()
{
    if (d->renderer)
        d->renderer->lockRenderer();
}

/* ---------------------------------------------------------------- *
   Restart the rendering when user has finished resizing the widget.
 * -----------------------------------------------------------------*/
void Widget::onWidgetResized()
{
    if (d->renderer)
        d->renderer->unlockRenderer();
}

/* ---------------------------------------------------------------- *
   Move the OpenGL context into renderer thread.
 * -----------------------------------------------------------------*/
void Widget::onRendererRequestContext()
{
    d->renderer->lockRenderer();
    QMutexLocker lock(d->renderer->requestWaitMutex());
    context()->moveToThread(d->thread.get());
    d->renderer->requestWaitCondition()->wakeAll();
    d->renderer->unlockRenderer();
}

/* ---------------------------------------------------------------- *
   Paint event needs to be overridden for rendering to work.
 * -----------------------------------------------------------------*/
void Widget::paintEvent(QPaintEvent* /*e*/)
{}

/* ---------------------------------------------------------------- *
   Stop the thread during exit.
 * -----------------------------------------------------------------*/
void Widget::closeEvent(QCloseEvent* /*e*/)
{
    stopThread();
}

} // namespace opengl
} // namespace kuu
