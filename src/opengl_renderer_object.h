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
 * ---------------------------------------------------------------- */
class RendererObject : public QObject
{
    Q_OBJECT

public:
    // Defines a shared pointer of renderer object.
    using Ptr = std::shared_ptr<RendererObject>;

    // Constructs the renderer object.
    RendererObject(Widget::WeakPtr widget);

    void exit();

    void lockRenderer();
    void unlockRenderer();

    QMutex* requestWaitMutex() const;
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
