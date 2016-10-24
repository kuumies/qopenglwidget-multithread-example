/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Implementation of kuu::opengl::RendererObject class.
 * ---------------------------------------------------------------- */

#include "opengl_renderer_object.h"
#include "opengl_quad.h"
#include "opengl_widget.h"
#include <chrono>
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLContext>

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *
   A simple timer for getting the elapsed time in milliseconds.
 * ---------------------------------------------------------------- */
class ElapsedTimer
{
public:
    // Shorthand aliases of clock
    using Clock = std::chrono::steady_clock;
    using ClockTimePoint = Clock::time_point;

    // Constructs the elapsed timer
    ElapsedTimer()
    {
        prevTime_ = Clock::now();
    }

    // Returns the elapsed time in milliseconds since the function
    // was previously called.
    int elapsed()
    {

        ClockTimePoint currentTime = Clock::now();
        auto diffTime = currentTime - prevTime_;
        prevTime_ = currentTime;

        using namespace std::chrono;
        return duration_cast<milliseconds>(diffTime).count();
    }

private:
    ClockTimePoint prevTime_; // previous sampling time
};

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
    bool initialized = false;
    ElapsedTimer timer;
    Quad::Ptr quad;
};

/* ---------------------------------------------------------------- *
   Constructs the renderer object.
 * -----------------------------------------------------------------*/
RendererObject::RendererObject(Widget::WeakPtr widget)
    : d(std::make_shared<Data>(widget))
{}

/* ---------------------------------------------------------------- *
   Stops the rendering
 * -----------------------------------------------------------------*/
void RendererObject::stop()
{
    d->exiting = true;
    d->requestWaitCondition.wakeAll();
}

/* ---------------------------------------------------------------- *
   Locks the rendering mutex
 * -----------------------------------------------------------------*/
void RendererObject::lockRenderer()
{
    d->renderMutex.lock();
}

/* ---------------------------------------------------------------- *
   Unlocks the rendering mutex
 * -----------------------------------------------------------------*/
void RendererObject::unlockRenderer()
{
    d->renderMutex.unlock();
}

/* ---------------------------------------------------------------- *
   Returns the request context mutex
 * -----------------------------------------------------------------*/
QMutex* RendererObject::requestWaitMutex() const
{
    return &d->requestWaitMutex;
}

/* ---------------------------------------------------------------- *
   Returns wait condition of the request context mutex
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

    // Initialize OpenGL if needed.
    if (!d->initialized)
    {
#ifdef _WIN32
        glewExperimental = GL_TRUE;
        GLenum result = glewInit();
        if (result != GLEW_OK)
        {
            std::cerr << "Failed to initialize GLEW."
                      << std::endl;
            return;
        }
#endif
        d->quad = std::make_shared<Quad>(2.0f, 2.0f);
        d->initialized = true;
    }

    // Perspective projection matrix
    float width  = 720.0f;
    float height = 576.0f;
    const float aspect = float(width) / float(height);
    const glm::mat4 projection =
        glm::perspective(glm::radians(45.0f),
                         aspect, 0.1f, 10.0f);

    // View matrix
    const glm::mat4 view =
        glm::translate(glm::mat4(1.0f),
                       glm::vec3(0.0f, 0.0f, -5.0f));

    // Clear the color buffer
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Render the quad
    d->quad->update(d->timer.elapsed());
    d->quad->render(view, projection);

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
