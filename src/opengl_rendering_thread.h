/**
    @file   opengl_rendering_thread.h
    @author kuumies <kuumies@gmail.com>
    @brief  Definition of kuu::opengl::RenderingThread class.
 **/

#pragma once

#include <memory>
#include <QtCore/QThread>
#include "opengl_widget.h"

namespace kuu
{
namespace opengl
{

/**
   @brief   A rendering thread for the OpenGL widget.

   @details This thread renders a rotating triangle into framebuffer
            by using OpenGL 3.3 pipeline. The framebuffer can then be
            shown to user in UI thread.

            The thread uses double-buffering meaning a framebuffer
            can be shown to user while the next frame rendering is
            going on.

            The framebuffer can be accessed as a two-dimensional
            texture by getting the texture ID with @ref
            framebufferTexture function. Before that the thread must
            be locked with @ref lock function and after rendering
            unlocked with @ref unlock call.
 **/
class RenderingThread : public QThread
{
public:
    /**
        @brief Constructs the rendering thread.

        @param widget The OpenGL widget. After a frame is rendered
                      the thread will call 'update' slot for widget
                      to paint its surface. The size of the widget is
                      also used as the size of framebuffers.
     **/
    RenderingThread(Widget* widget);

    /**
       @brief   Stop sthe rendering thread.
       @details The thread is not stopped immediately but before the
                next frame is rendered.
     */
    void stop();

    /**
        @brief   Locks the rendering thread mutex.
        @details The framebuffer texture can then be accessed via
                 @ref framebufferTexture function.
     **/
    void lock();

    /**
       @brief Unlocks the rendering thread mutex.
     **/
    void unlock();

    /**
       @brief Returns the framebuffer texture ID.
     **/
    GLuint framebufferTexture() const;

protected:
    void run();

public:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
