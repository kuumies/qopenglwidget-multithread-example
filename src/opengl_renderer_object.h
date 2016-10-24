/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Definition of kuu::opengl::RendererObject class.
 * ---------------------------------------------------------------- */

#pragma once

#include <memory>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QWaitCondition>
#include "opengl_widget.h"

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *
   A renderer object.

   TODO: find a better way to do the synchronization with the
         widget. Now it is super ugly, super hackish looking.
 * ---------------------------------------------------------------- */
class RendererObject : public QObject
{
    Q_OBJECT

public:
    // Defines a shared pointer of renderer object.
    using Ptr = std::shared_ptr<RendererObject>;

    // Constructs the renderer object.
    RendererObject(Widget::WeakPtr widget);

    // Stops the rendering
    void stop();

    // Locks the rendering mutex
    void lockRenderer();
    // Unlocks the rendering mutex
    void unlockRenderer();

    // Returns the request context mutex
    QMutex* requestWaitMutex() const;
    // Returns wait condition of the request context mutex
    QWaitCondition* requestWaitCondition() const;

public slots:
    // Render a frame
    void render();

signals:
    // Request the context is moved into this thread.
    void requestContext();

private:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
