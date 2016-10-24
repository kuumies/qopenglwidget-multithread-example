/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Implementation of kuu::opengl::RendererObject class.
 * ---------------------------------------------------------------- */

#include "opengl_renderer_object.h"
#include "opengl_widget.h"
#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLContext>

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *
   The data of the renderer object.
 * ---------------------------------------------------------------- */
struct RendererObject::Data
{
    Data(Widget::WeakPtr widget)
        : widget(widget)
    {}

    Widget::WeakPtr widget;
    QWaitCondition requestWaitCondition;
    QMutex requestWaitMutex;
    QMutex renderMutex;
    bool exiting = false;
};

/* ---------------------------------------------------------------- *
   Constructs the renderer object.
 * -----------------------------------------------------------------*/
RendererObject::RendererObject(Widget::WeakPtr widget)
    : d(std::make_shared<Data>(widget))
{}

/* ---------------------------------------------------------------- *

 * -----------------------------------------------------------------*/
void RendererObject::exit()
{
    d->exiting = true;
    d->requestWaitCondition.wakeAll();
}

/* ---------------------------------------------------------------- *

 * -----------------------------------------------------------------*/
void RendererObject::lockRenderer()
{
    d->renderMutex.lock();
}

/* ---------------------------------------------------------------- *

 * -----------------------------------------------------------------*/
void RendererObject::unlockRenderer()
{
    d->renderMutex.unlock();
}

/* ---------------------------------------------------------------- *

 * -----------------------------------------------------------------*/
QMutex* RendererObject::requestWaitMutex() const
{
    return &d->requestWaitMutex;
}

/* ---------------------------------------------------------------- *

 * -----------------------------------------------------------------*/
QWaitCondition* RendererObject::requestWaitCondition() const
{
    return &d->requestWaitCondition;
}

/* ---------------------------------------------------------------- *
   Renderers a frame.
 * -----------------------------------------------------------------*/
void RendererObject::render()
{
    Widget::Ptr w = d->widget.lock();
    if (!w)
        return;

    QOpenGLContext *ctx = w->context();
    if (!ctx) // QOpenGLWidget not yet initialized
        return;

    // Request that the widget moves the OpenGL context
    // into this thread.
    d->requestWaitMutex.lock();
    emit requestContext();
    d->requestWaitCondition.wait(&d->requestWaitMutex);
    QMutexLocker lock(&d->renderMutex);
    d->requestWaitMutex.unlock();

    if (d->exiting)
        return;

    w->makeCurrent();

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Make no context current on this thread and move the QOpenGLWidget's
    // context back to the gui thread.
    w->doneCurrent();
    ctx->moveToThread(qGuiApp->thread());

    // Schedule composition. Note that this will use QueuedConnection, meaning
    // that update() will be invoked on the gui thread.
    QMetaObject::invokeMethod(w.get(), "update");
}

} // namespace opengl
} // namespace kuu
