/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Definition of kuu::opengl::RendererObject class.
 * ---------------------------------------------------------------- */

#pragma once

#include <memory>
#include <QtCore/QObject>
#include "opengl.h"

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
    RendererObject();


public slots:
    // Render a frame
    void renderer();

private:
    struct Data;
    std::shared_ptr<Data> d;
};

} // namespace opengl
} // namespace kuu
