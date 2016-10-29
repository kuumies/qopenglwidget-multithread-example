/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Definition of kuu::opengl::RenderingThread class.
 * ---------------------------------------------------------------- */

#pragma once

#include <memory>
#include <QtCore/QThread>
#include "opengl_widget.h"

class QOffscreenSurface;

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *
   A thread that renders a rotating triangle into framebuffer by
   using OpenGL 3.3 pipeline. The framebuffer can then be shown
   to user in UI thread.
 * ---------------------------------------------------------------- */
class RenderingThread : public QThread
{
public:
    // Defines a shared pointer of rendering thread.
    using Ptr = std::shared_ptr<RenderingThread>;

    // Constructs the rendering thread.
    RenderingThread(Widget* widget, const QSize& framebufferSize);

    // Stop the rendering thread
    void stop();

    // Locks the rendering thread mutex. The framebuffer texture
    // can then be accessed via framebufferTex function.
    void lock();
    // Unlocks the rendering thread mutex.
    void unlock();

    // Returns the framebuffer texture.
    GLuint framebufferTex() const;

protected:
    void run();

public:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
