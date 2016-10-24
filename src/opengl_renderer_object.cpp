/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Implementation of kuu::opengl::RendererObject class.
 * ---------------------------------------------------------------- */

#include "opengl_renderer_object.h"

namespace kuu
{
namespace opengl
{

/* ---------------------------------------------------------------- *
   The data of the renderer object.
 * ---------------------------------------------------------------- */
struct RendererObject::Data
{
};

/* ---------------------------------------------------------------- *
   Constructs the renderer object.
 * -----------------------------------------------------------------*/
RendererObject::RendererObject()
    : d(std::make_shared<Data>())
{}

/* ---------------------------------------------------------------- *
   Renderers a frame.
 * -----------------------------------------------------------------*/
void RendererObject::renderer()
{

}

} // namespace opengl
} // namespace kuu
