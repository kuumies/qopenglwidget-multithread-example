/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Implementation of kuu::opengl::RenderingThread class.
 * ---------------------------------------------------------------- */

#include "opengl_rendering_thread.h"
#include "opengl_quad.h"
#include "opengl_widget.h"

#include <chrono>
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <QtGui/QOffscreenSurface>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>

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
struct RenderingThread::Data
{
    Data(Widget* widget, const QSize& framebufferSize)
        : widget(widget)
        , framebufferSize(framebufferSize)
    {}

    // OpenGL context
    std::shared_ptr<QOpenGLContext> context;
    // Offscreen surface
    std::shared_ptr<QOffscreenSurface> surface;
    // OpengL widget
    Widget* widget;
    // Size of framebuffers
    QSize framebufferSize;

    // Rendering mutex
    QMutex mutex;
    // True if the application is exiting
    bool exiting = false;
    // True if the OpenGL is initialized
    bool initialized = false;
    // Timer for rotating the quad.
    ElapsedTimer timer;
    // Quad mesh
    Quad::Ptr quad;

    // Framebuffer texture ID for the UI thread.
    GLuint tex = 0;
    // Framebuffer for thread to render the rotating quad.
    std::shared_ptr<QOpenGLFramebufferObject> renderFbo;
    // Framebuffer for UI to display
    std::shared_ptr<QOpenGLFramebufferObject> displayFbo;
};

/* ---------------------------------------------------------------- *
   Initialize OpenGL. This will create a quad mesh and the
   framebuffers for double-buffering.
 * -----------------------------------------------------------------*/
void initialize(std::shared_ptr<RenderingThread::Data> d)
{
    // Initialize OpenGL if needed.
    if (d->initialized)
        return;

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
    // Create the rotating quad
    d->quad = std::make_shared<Quad>(2.0f, 2.0f);

    // Create the framebuffer objects. Two framebuffers is needed
    // for double-buffering.

    QOpenGLFramebufferObjectFormat framebufferFormat;
    framebufferFormat.setAttachment(
        QOpenGLFramebufferObject::CombinedDepthStencil);

    d->renderFbo  = std::make_shared<QOpenGLFramebufferObject>(
        d->framebufferSize,
        framebufferFormat);

    d->displayFbo = std::make_shared<QOpenGLFramebufferObject>(
        d->framebufferSize,
        framebufferFormat);

    d->initialized = true;
}

/* ---------------------------------------------------------------- *
   Renders a frame
 * -----------------------------------------------------------------*/

void renderFrame(std::shared_ptr<RenderingThread::Data> d)
{
    // Bind the framebuffer for rendering.
    d->renderFbo->bind();

    // Set the viewport
    const QSize size = d->framebufferSize;
    glViewport(0, 0, size.width(), size.height());

    // Perspective projection matrix
    const float aspect = float(size.width()) / float(size.height());
    const glm::mat4 projection =
        glm::perspective(
            glm::radians(45.0f), aspect, 0.1f, 10.0f);

    // View matrix
    const glm::mat4 view =
        glm::translate(glm::mat4(1.0f),
                       glm::vec3(0.0f, 0.0f, -5.0f));

    // Clear the color buffer
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set rendering attributes
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Render the quad
    d->quad->update(d->timer.elapsed());
    d->quad->render(view, projection);

    // Flush the pipeline
    glFlush();

    // Release the framebuffer
    d->renderFbo->release();
    // Take the current framebuffer texture ID
    d->tex = d->renderFbo->texture();

    // Swap the framebuffers for double-buffering.
    std::swap(d->renderFbo, d->displayFbo);
}

/* ---------------------------------------------------------------- *
   Constructs the renderer object.
 * -----------------------------------------------------------------*/
RenderingThread::RenderingThread(Widget* widget,
                                 const QSize& framebufferSize)
    : d(std::make_shared<Data>(widget, framebufferSize))
{
    d->context = std::make_shared<QOpenGLContext>();
    d->context->setShareContext(widget->context());
    d->context->setFormat(widget->context()->format());
    d->context->create();
    d->context->moveToThread(this);

    d->surface = std::make_shared<QOffscreenSurface>();
    d->surface->setFormat(d->context->format());
    d->surface->create();
    d->surface->moveToThread(this);
}

/* ---------------------------------------------------------------- *
   Stops the rendering
 * -----------------------------------------------------------------*/
void RenderingThread::stop()
{
    d->mutex.lock();
    d->exiting = true;
    d->mutex.unlock();
}

/* ---------------------------------------------------------------- *
   Locks the rendering mutex
 * -----------------------------------------------------------------*/
void RenderingThread::lock()
{
    d->mutex.lock();
}

/* ---------------------------------------------------------------- *
   Unlocks the rendering mutex
 * -----------------------------------------------------------------*/
void RenderingThread::unlock()
{
    d->mutex.unlock();
}

/* ---------------------------------------------------------------- *
   Returns the current framebuffer texture ID.
 * -----------------------------------------------------------------*/
GLuint RenderingThread::framebufferTex() const
{
    return d->tex;
}

/* ---------------------------------------------------------------- *
   Renderers a frame.
 * -----------------------------------------------------------------*/
void RenderingThread::run()
{
    for(;;)
    {
        // Lock the rendering mutex.
        QMutexLocker lock(&d->mutex);

        // Stops the thread if exit flag is set.
        if (d->exiting)
            break;

        // Make the OpenGL context current on offscreen surface.
        d->context->makeCurrent(d->surface.get());

        // Initialize if not done.
        if (!d->initialized)
        {
            initialize(d);
            d->initialized = true;
        }

        // Renders the frame
        renderFrame(d);

        // Release OpenGL context
        d->context->doneCurrent();

        // Notify UI about new frame.
        QMetaObject::invokeMethod(d->widget, "update");
    }
}

} // namespace opengl
} // namespace kuu
